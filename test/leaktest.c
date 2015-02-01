#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "TCSImpl.h"
#include "TCSErrorCodes.h"
#include "TCSTest.h"
#include "TWPImpl.h"
#include "XMHttp.h"

#if !defined(MIN)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#define EICAR_TEST "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"
#define NONMALICIOUS_TEST "HOW CLEAN THE DATA IS!"
#define CLEAN_TESTURL "http://www.samsung.com"


TWPCategories CATEGORIES[3] =
{
    TWP_OverallRiskHigh,
    TWP_OverallRiskMedium,
    TWP_OverallRiskMinimal
};


typedef struct TRequest
{
    TWPRequest Request;
    const char *pszUrl;
    size_t ResponseLength;
    char *ResponseBody;
    XM_HTTP_HANDLE hHttp;
    char *pData;
    unsigned int uLength;
    unsigned int uRead;
    size_t ResponseBytesRead;
} TRequest;


typedef struct ScanPrivate
{
    const char *pszData;
    unsigned int uPos;
    unsigned int uSize;
} ScanPrivate;


typedef int (*LeakTestFunc)(const char *);

typedef struct LeakTestHarness
{
    int id;
    LeakTestFunc TestFunc;
    const char *pszTestName;
} LeakTestHarness;


static int tcsleaktest_001(const char *param);
static int tcsleaktest_002(const char *param);
static int tcsleaktest_003(const char *param);
static int tcsleaktest_004(const char *param);

static int twpleaktest_001(const char *param);
static int twpleaktest_002(const char *param);
static int twpleaktest_003(const char *param);


static LeakTestHarness TestHarnesses [] = {
   {1, tcsleaktest_001, "tcsleaktest_001"},
   {2, tcsleaktest_002, "tcsleaktest_002"},
   {3, tcsleaktest_003, "tcsleaktest_003"},
   {4, tcsleaktest_004, "tcsleaktest_004"},
   {5, twpleaktest_001, "twpleaktest_001"},
   {6, twpleaktest_002, "twpleaktest_002"},
   {7, twpleaktest_003, "twpleaktest_003"},
   {-1, NULL, NULL},
};


int getpss(char *pszline){
    char *start;
    while(*pszline <= 0x20)
        pszline++;
    if (*pszline == '\0')
        return 0;
    start = pszline;

    while(*pszline > 0x20)
        pszline++;

    *pszline = '\0';
    return atoi(start);
}


int gettotalpss(int pid){
    FILE *fp;
    int pss_size = 0;
    char buf[256];
    snprintf(buf, sizeof(buf) - 1, "/proc/%d/smaps", pid);
    if ((fp = fopen(buf, "r")) == NULL) {
        fprintf(stderr, "failed to open %s\n", buf);
        return -1;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL){
        if (strncasecmp(buf, "Pss:", 4) == 0){
            pss_size += getpss(buf + 4);
        }
    }
    fclose(fp);
    return pss_size;
}


static TWP_RESULT CbSetUrl(struct TWPRequest *pRequest, const char *pszUrl, unsigned int uLength)
{
    TRequest *pCtx = (TRequest *) pRequest;

    fprintf(stderr,"url is: %s\n", pszUrl);
    if (pCtx->pszUrl != NULL)
        free((void *) pCtx->pszUrl);
    pCtx->pszUrl = strdup(pszUrl);
    return pCtx->pszUrl ? TWP_SUCCESS : TWP_NOMEM;
}

static TWP_RESULT CbSetMethod(struct TWPRequest *pRequest, TWPSubmitMethod Method)
{
    return Method == TWPPOST ? TWP_SUCCESS : TWP_INVALID_PARAMETER;
}

static int CbHttpWrite(void *pPrivate, void const *pData, int iSize)
{
    TRequest *pCtx = (TRequest *) pPrivate;
    char *pTmp = NULL;

    fprintf(stderr,"[http] recv data\n");
    pTmp = (char *) realloc(pCtx->ResponseBody, pCtx->ResponseLength + iSize);
    if (pTmp == NULL)
    {
        fprintf(stderr,"failed to alloc mem\n");
        return 0;
    }

    pCtx->ResponseBody = pTmp;
    memcpy(pCtx->ResponseBody + pCtx->ResponseLength, pData, iSize);
    pCtx->ResponseLength += iSize;

    return iSize;
}

static int CbHttpRead(void *pPrivate, void *pData, int iSize)
{
    TRequest *pCtx = (TRequest *) pPrivate;
    unsigned int uToRead = MIN(iSize, pCtx->uLength - pCtx->uRead);

    fprintf(stderr,"[http] recv data\n");

    memcpy(pData, pCtx->pData + pCtx->uRead, uToRead);
    pCtx->uRead += uToRead;

    return (int) uToRead;
}

static long CbHttpGetSize(void *pPrivate)
{
    TRequest *pCtx = (TRequest *) pPrivate;

    fprintf(stderr,"[http] get size\n");

    return (long) (pCtx->uLength - pCtx->uRead);
}

static TWP_RESULT HttpSend(TRequest *pCtx, const void *pData, unsigned int uLength)
{
    int iRet;
    XmHttpCallbacks HttpCb;

    if (pCtx->hHttp == INVALID_XM_HTTP_HANDLE)
    {
        pCtx->hHttp = XmHttpOpen();
        if (pCtx->hHttp == NULL)
            return TWP_NOMEM;
    }

    HttpCb.pfWrite = CbHttpWrite;
    HttpCb.pfRead = CbHttpRead;
    HttpCb.pfGetSize = CbHttpGetSize;

    pCtx->pData = (char *) pData;
    pCtx->uLength = uLength;
    pCtx->uRead = 0;

    iRet = XmHttpExec(pCtx->hHttp, "POST", pCtx->pszUrl, &HttpCb, pCtx);

    fprintf(stderr,"http: iRet = %d\n", iRet);

    return iRet == 0 ? TWP_SUCCESS : TWP_ERROR;
}

static TWP_RESULT CbSend(struct TWPRequest *pRequest, TWPResponseHandle hResponse,
                         const void *pData, unsigned int uLength)
{
    fprintf(stderr,"send %s\n", (const char *)pData);

    return HttpSend((TRequest *) pRequest, pData, uLength);
}

static TWP_RESULT CbRecv(struct TWPRequest *pRequest, void *pBuffer, unsigned int uBufferLength,
                         unsigned int *puLength)
{
    TRequest* pCtx = (TRequest*) pRequest;
    size_t BytesToCopy = 0;

    fprintf(stderr,"recv\n");

    if ( pCtx->ResponseBytesRead < pCtx->ResponseLength )
    {
        BytesToCopy = uBufferLength < (pCtx->ResponseLength - pCtx->ResponseBytesRead) ? uBufferLength : (pCtx->ResponseLength - pCtx->ResponseBytesRead);
        memcpy(pBuffer, (void*) (pCtx->ResponseBody + pCtx->ResponseBytesRead), BytesToCopy);
        pCtx->ResponseBytesRead += BytesToCopy;
    }

    *puLength = (int) BytesToCopy;

    return TWP_SUCCESS;
}


static long GenerateRandomNumber()
{
    return rand();
}


static TCSOffset CbGetSize(void *pPrivate)
{
    ScanPrivate *pScanPrivate = (ScanPrivate *)pPrivate;
    if (pScanPrivate == NULL)
        return -1;
    return (TCSOffset)pScanPrivate->uSize;
}


static unsigned int CbRead(void *pPrivate, TCSOffset uOffset, void *pBuffer, unsigned int uCount)
{
    unsigned int uRead;

    ScanPrivate *pScanPrivate = (ScanPrivate *)pPrivate;
    if (pScanPrivate == NULL)
        return -1;
    if (uOffset + uCount > pScanPrivate->uSize)
        uRead = (TCSOffset)pScanPrivate->uSize - uOffset;
    else
        uRead = uCount;
    memcpy(pBuffer, (pScanPrivate->pszData + uOffset), uRead);
    return uRead;
}


static int CbCallBack(void *pPrivate, int iReason, void *pParam)
{
    return 0;
}

static int tcsscan(TCSLIB_HANDLE hLib, const char* pszScanData)
{
    TCSScanParam SP = {0};
    TCSScanResult SR = {0};
    ScanPrivate Private = {0};
    Private.pszData = pszScanData;
    Private.uSize = strlen(pszScanData);
    Private.uPos = 0;
    SP.iAction = TCS_SA_SCANONLY;
    SP.iDataType = TCS_DTYPE_UNKNOWN;
    SP.iCompressFlag = 0;
    SP.pPrivate = &Private;
    SP.pfGetSize = CbGetSize;
    SP.pfRead = CbRead;
    SP.pfCallBack = CbCallBack;

    int ret = TCSScanData(hLib, &SP, &SR);
    if (ret == 0)
    {
        ret = SR.iNumDetected;
        if (SR.iNumDetected > 0)
            SR.pfFreeResult(&SR);
    }
    return ret;
}

static int tcsscanfile(TCSLIB_HANDLE hLib, const char* pszFilePath)
{
    TCSScanResult SR = {0};
    int ret = TCSScanFile(hLib, pszFilePath, TCS_DTYPE_UNKNOWN, TCS_SA_SCANONLY, 1, &SR);
    if (ret == 0)
    {
        ret = SR.iNumDetected;
        if (SR.iNumDetected > 0)
            SR.pfFreeResult(&SR);
    }
    return ret;
}

static TWP_RESULT twpcreate_config(TWPLIB_HANDLE hWpLib, TWPConfigurationHandle *phCfg)
{
    TWPConfiguration Cfg;
    Cfg.config_version = TWPCONFIG_VERSION;
    Cfg.client_id = NULL;
    Cfg.client_key = NULL;
    Cfg.host = NULL;
    Cfg.secure_connection = 0;
    Cfg.skip_dla = 0;
    Cfg.obfuscate_request = 1;
    Cfg.randomfunc = GenerateRandomNumber;
    return TWPConfigurationCreate(hWpLib, &Cfg, phCfg);
}


static TWP_RESULT twprating(TWPLIB_HANDLE hWpLib, TWPConfigurationHandle hCfg, const char *url, int *pscore)
{
    TWPResponseHandle hResponse = NULL;
    TRequest Request;
    TWPUrlRatingHandle hRating = NULL;
    TWPPolicyHandle hPolicy = NULL;
    const char *Urls[1];
    Urls[0] = url;
    *pscore = 255;
    int iRet;

    memset(&Request, 0, sizeof(Request));
    Request.Request.request_version = TWPREQUEST_VERSION;
    Request.Request.seturlfunc = CbSetUrl;
    Request.Request.setmethodfunc = CbSetMethod;
    Request.Request.sendfunc = CbSend;
    Request.Request.receivefunc = CbRecv;

    iRet = TWPLookupUrls(hWpLib, hCfg, &Request.Request, 1, Urls, sizeof(Urls) / sizeof(const char *), &hResponse);
    fprintf(stderr,"TWPPLookupUrls : iRet = %d\n", iRet);

    if (Request.hHttp != INVALID_XM_HTTP_HANDLE)
        XmHttpClose(Request.hHttp);

    iRet = TWPResponseGetUrlRatingByUrl(hWpLib, hResponse, Urls[0], strlen(Urls[0]), &hRating);
    if (iRet != TWP_SUCCESS || hRating == NULL)
    {
        fprintf(stderr,"failed to get rating\n");
        goto finish;
    }

    fprintf(stderr,"create category = %d\n", sizeof(CATEGORIES) / sizeof(TWPCategories));
    iRet = TWPPolicyCreate(hWpLib, hCfg, CATEGORIES, sizeof(CATEGORIES) / sizeof(TWPCategories), &hPolicy);
    if (iRet != TWP_SUCCESS || hPolicy == NULL)
    {
        fprintf(stderr,"failed to create policy\n");
        goto finish;
    }

    int iScore = 0;
    iRet = TWPUrlRatingGetScore(hWpLib, hRating, &iScore);
    if (iRet != TWP_SUCCESS)
    {
        *pscore = 255;
        fprintf(stderr,"failed to get score\n");
    }
    else
    {
        *pscore = iScore;
        fprintf(stderr,"score = %d for %s\n", iScore, Urls[0]);
    }

finish:
    if (hResponse != NULL)
        TWPResponseDestroy(hWpLib, &hResponse);

    if (hPolicy != NULL)
        TWPPolicyDestroy(hWpLib, &hPolicy);

    if (Request.ResponseBody != NULL)
        free(Request.ResponseBody);
    if (Request.pszUrl != NULL)
        free((void *)Request.pszUrl);

    return iRet;
}


static TWPLIB_HANDLE twpinit()
{
    TWPAPIInit Init;
    Init.api_version = TWPAPI_VERSION;
    Init.memallocfunc = (TWPFnMemAlloc) malloc;
    Init.memfreefunc = free;
    return TWPInitLibrary(&Init);
}


static int tcsleaktest_001(const char *param)
{
    TCSLIB_HANDLE hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;
    TCSLibraryClose(hLib);
    return 0;
}

static int tcsleaktest_002(const char *param)
{
    TCSLIB_HANDLE hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;
    if (param == NULL)
        param = EICAR_TEST;

    int iRet = tcsscan(hLib, param);
    TCSLibraryClose(hLib);
    return iRet;
}

static int tcsleaktest_003(const char *param)
{
    TCSLIB_HANDLE hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;
    if (param == NULL)
        param = NONMALICIOUS_TEST;

    int iRet = tcsscan(hLib, param);
    TCSLibraryClose(hLib);
    return iRet;
}

static int tcsleaktest_004(const char *param)
{
    assert(param != NULL);
    if (param == NULL)
        return -1;
    TCSLIB_HANDLE hLib = TCSLibraryOpen();
    if (hLib == INVALID_TCSLIB_HANDLE)
        return -1;
    int iRet = tcsscanfile(hLib, param);
    TCSLibraryClose(hLib);
    return iRet;
}

static  int twpleaktest_001(const char *param)
{
    TWPLIB_HANDLE hLib = twpinit();
    if (hLib == INVALID_TWPLIB_HANDLE)
        return -1;
    TWPUninitLibrary(hLib);
    return 0;
}


static  int twpleaktest_002(const char *param)
{
    TWPLIB_HANDLE hLib = twpinit();
    TWPConfigurationHandle hCfg = NULL;
    if (hLib == INVALID_TWPLIB_HANDLE)
        return -1;

    int iRet = twpcreate_config(hLib, &hCfg);
    if (hCfg != NULL)
        TWPConfigurationDestroy(hLib, &hCfg);

    TWPUninitLibrary(hLib);
    return iRet;
}

static  int twpleaktest_003(const char *param)
{
    TWPLIB_HANDLE hLib = twpinit();
    int iScore;
    TWPConfigurationHandle hCfg = NULL;
    if (hLib == INVALID_TWPLIB_HANDLE)
        return -1;

    if (param == NULL)
        param = CLEAN_TESTURL;

    int iRet = twpcreate_config(hLib, &hCfg);
    if (iRet == TWP_SUCCESS)
        iRet = twprating(hLib, hCfg, param, &iScore);
    if (hCfg != NULL)
        TWPConfigurationDestroy(hLib, &hCfg);
    TWPUninitLibrary(hLib);
    return iRet;
}

static int runtest(int testnum, int interval, LeakTestHarness *harness, const char *param)
{
    int i;
    int iRet;
    int trynum = 0;
    int pss;
    pss = gettotalpss(getpid());
    fprintf(stdout, "START,%d\n", pss);
    sleep(3);
    for(i = 1; i<= testnum; i++)
    {
        iRet = harness->TestFunc(param);
        if (++trynum == interval){
            trynum = 0;
            pss = gettotalpss(getpid());
            fprintf(stdout, "%d,%d,%d\n", i, pss, iRet);
        }
    }
    sleep(3);
    pss = gettotalpss(getpid());
    fprintf(stdout, "END,%d\n", pss);
    return 0;
}


void usage(const char *prog, LeakTestHarness * harness)
{
    int i;
    fprintf(stderr, "%s <testid> <test num> <sampling interval> <param for test>\n", prog);
    for(i = 0; harness[i].pszTestName != NULL; i++){
        fprintf(stderr, "%d: %s\n", harness[i].id, harness[i].pszTestName);
    }
}


int findtestharness(int id, LeakTestHarness *harness)
{
    int i;
    for(i = 0; harness[i].TestFunc != NULL; i++)
    {
       if (harness[i].id == id)
           return i;
    }
    return -1;
}


int main(int argc, char** argv)
{
    int testnum;
    int interval;
    int testid;
    int harnessid;
    char *param = NULL;
    if (argc < 4)
    {
        usage(argv[0], TestHarnesses);
        return 1;
    }
    testid = atoi(argv[1]);
    testnum = atoi(argv[2]);
    interval = atoi(argv[3]);
    if (argc > 4)
        param = argv[4];

    if (testid == 0 || testnum == 0 || interval == 0)
    {
        fprintf(stderr, "invalid number");
        usage(argv[0], TestHarnesses);
        return 2;
    }
    else if (testnum < interval){
        fprintf(stderr, "sampling interval is greater than test num");
        usage(argv[0], TestHarnesses);
        return 2;
    }
    else if ((harnessid = findtestharness(testid, TestHarnesses)) < 0){
        fprintf(stderr, "%d is not found", testid);
        usage(argv[0], TestHarnesses);
        return 2;
    }

    runtest(testnum, interval, &TestHarnesses[harnessid], param);

    return 0;
}

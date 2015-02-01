#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
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


static int runscan(TCSLIB_HANDLE hLib, int dtype, const char* filepath){
    int ret = 0;
    TCSScanResult SR = {0};
    TCSDetected *pDetected = NULL;

    fprintf(stderr, "Scan start\n");
    ret = TCSScanFile(hLib, filepath, dtype, 1, TCS_SA_SCANONLY, &SR);
    if (ret < 0){
        fprintf(stdout, "%s, -1\n", filepath);
        return -1;
    }
    ret = SR.iNumDetected;
    fprintf(stderr, "scan completed\n");
    if (SR.iNumDetected > 0){
        pDetected = SR.pDList;
        fprintf(stderr, "TCSDetected = %p\n", pDetected);
        fprintf(stdout, "%s, %d, %s, %s, %d, %d, %d\n", filepath, SR.iNumDetected, 
                pDetected->pszName, 
                pDetected->pszVariant == NULL ? "(NULL)":pDetected->pszVariant, 
                pDetected->uType, (pDetected->uAction >> 8) & 0xFF,
                (pDetected->uAction) & 0xFF);
    }else{
        fprintf(stdout, "%s, 0\n", filepath);
    }
    fprintf(stderr, "cleanup result\n");
    SR.pfFreeResult(&SR);
    fprintf(stderr, "Scan end\n");
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
    TWPResponseHandle hResponse;
    TRequest Request;
    TWPUrlRatingHandle hRating;
    TWPPolicyHandle hPolicy;
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
        TWPResponseDestroy(hWpLib, &hResponse);
        return -1;
    }

    fprintf(stderr,"create category = %d\n", sizeof(CATEGORIES) / sizeof(TWPCategories));
    iRet = TWPPolicyCreate(hWpLib, hCfg, CATEGORIES, sizeof(CATEGORIES) / sizeof(TWPCategories), &hPolicy);
    if (iRet != TWP_SUCCESS || hPolicy == NULL)
    {
        fprintf(stderr,"failed to create policy\n");
        TWPResponseDestroy(hWpLib, &hResponse);
        return -1;
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
    TWPResponseDestroy(hWpLib, &hResponse);
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


void usage(const char *prog)
{
    fprintf(stderr, "%s <virus|web|loadlib> [option]\n", prog);
    fprintf(stderr, "    virus:<data type 0-7> [path ...]\n");
    fprintf(stderr, "    web:<url 1>, [url 2] ...\n");
    fprintf(stderr, "    loadlib:<path to plugin library>\n");
}


int main(int argc, char** argv)
{
    int i;
    if (argc <= 1){
        usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "virus") == 0)
    {
        TCSLIB_HANDLE hLib;
        if (argc < 4){
            usage(argv[0]);
            return 1;
        }

        int dtype = atoi(argv[2]);
        if (dtype < 0 || dtype > 7){
            fprintf(stderr, "invalid data type = %d\n", dtype);
            return 2;
        }

        hLib = TCSLibraryOpen();
        if (hLib == INVALID_TCSLIB_HANDLE){
            fprintf(stderr, "TCSLibraryOpen() fails\n");
            return 3;
        }
        for (i = 3; i< argc; i++){
            runscan(hLib, dtype, argv[i]); 
        }

        if (hLib != INVALID_TCSLIB_HANDLE)
            TCSLibraryClose(hLib);
    }
    else if (strcmp(argv[1], "web") == 0)
    {
        int iRet;
        TWPLIB_HANDLE hWpLib;
        TWPConfigurationHandle hCfg = NULL;
        int iScore;
        if (argc < 3){
            usage(argv[0]);
            return 1;
        }
        hWpLib = twpinit();
        if (hWpLib == INVALID_TWPLIB_HANDLE){
            fprintf(stderr, "TWPInitLibrary() fails\n");
            return 3;
        }
        if ((iRet = twpcreate_config(hWpLib, &hCfg)) != TWP_SUCCESS){
            TWPUninitLibrary(hWpLib);
            return 3;
        }
        if ((iRet = twprating(hWpLib, hCfg, argv[2], &iScore)) != TWP_SUCCESS){
            TWPConfigurationDestroy(hWpLib, &hCfg);
            TWPUninitLibrary(hWpLib);
            return 3;
        }
        fprintf(stderr, "%d:%s\n", iScore, argv[2]);
        TWPConfigurationDestroy(hWpLib, &hCfg);
        TWPUninitLibrary(hWpLib);
    }else if (strcmp(argv[1], "loadlib") == 0){
        if (argc < 3){
            usage(argv[0]);
            return 1;
        }
        void *pTmp = dlopen(argv[2], RTLD_LAZY);
        if (pTmp == NULL){
            fprintf(stdout, "%s: failed\n%s", argv[2], dlerror());
        }else{
            fprintf(stdout, "%s: OK\n", argv[2]);
            dlclose(pTmp);
        }
    }else{
        fprintf(stderr, "unknown command %s\n", argv[1]);
        usage(argv[0]);
        return 4;
    }
    return 0;
}

/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tet_api.h>
#include <stdio.h>
#include <stdlib.h>

#include <TCSImpl.h>
#include <TWPImpl.h>
#include <TCSErrorCodes.h>

enum {
    POSITIVE_TC_IDX = 0x01,
    NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;


static void utc_csr_framework_TCSGetLastError_n001(void);
static void utc_csr_framework_TCSLibraryClose_n001(void);
static void utc_csr_framework_TCSLibraryOpen_n001(void);
static void utc_csr_framework_TCSScanData_n001(void);
static void utc_csr_framework_TCSScanFile_n001(void);
static void utc_csr_framework_TWPConfigurationCreate_n001(void);
static void utc_csr_framework_TWPConfigurationDestroy_n001(void);
static void utc_csr_framework_TWPInitLibrary_n001(void);
static void utc_csr_framework_TWPLookupUrls_n001(void);
static void utc_csr_framework_TWPPolicyCreate_n001(void);
static void utc_csr_framework_TWPPolicyDestroy_n001(void);
static void utc_csr_framework_TWPPolicyGetViolations_n001(void);
static void utc_csr_framework_TWPPolicyValidate_n001(void);
static void utc_csr_framework_TWPResponseDestroy_n001(void);
static void utc_csr_framework_TWPResponseGetRedirUrlFor_n001(void);
static void utc_csr_framework_TWPResponseGetUrlRatingByIndex_n001(void);
static void utc_csr_framework_TWPResponseGetUrlRatingByUrl_n001(void);
static void utc_csr_framework_TWPResponseGetUrlRatingsCount_n001(void);
static void utc_csr_framework_TWPResponseWrite_n001(void);
static void utc_csr_framework_TWPUninitLibrary_n001(void);
static void utc_csr_framework_TWPUrlRatingGetCategories_n001(void);
static void utc_csr_framework_TWPUrlRatingGetDLAUrl_n001(void);
static void utc_csr_framework_TWPUrlRatingGetScore_n001(void);
static void utc_csr_framework_TWPUrlRatingGetUrl_n001(void);
static void utc_csr_framework_TWPUrlRatingHasCategory_n001(void);


struct tet_testlist tet_testlist[] = {
    { utc_csr_framework_TCSGetLastError_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TCSLibraryClose_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TCSLibraryOpen_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TCSScanData_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TCSScanFile_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPConfigurationCreate_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPConfigurationDestroy_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPInitLibrary_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPLookupUrls_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPPolicyCreate_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPPolicyDestroy_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPPolicyGetViolations_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPPolicyValidate_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPResponseDestroy_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPResponseGetRedirUrlFor_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPResponseGetUrlRatingByIndex_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPResponseGetUrlRatingByUrl_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPResponseGetUrlRatingsCount_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPResponseWrite_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPUninitLibrary_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPUrlRatingGetCategories_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPUrlRatingGetDLAUrl_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPUrlRatingGetScore_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPUrlRatingGetUrl_n001, NEGATIVE_TC_IDX},
    { utc_csr_framework_TWPUrlRatingHasCategory_n001, NEGATIVE_TC_IDX},
    { NULL, 0 }, 
};


static void startup(void)
{
    /* start of TC */
    tet_printf("\n TC start");
}


static void cleanup(void)
{
    /* end of TC */
    tet_printf("\n TC end");
}


/**
 * @brief Negative test case of TCSGetLastError()
 */
static void utc_csr_framework_TCSGetLastError_n001(void)
{
    TCSErrorCode rtn = (TCSErrorCode)0xdeadbeef;
    rtn = TCSGetLastError(INVALID_TCSLIB_HANDLE);
    dts_check_eq("TCSGetLastError", rtn, 0x01000007, "TCSGetLastError returned unexpected value.");
}


/**
 * @brief Negative test case of TCSLibraryClose()
 */
static void utc_csr_framework_TCSLibraryClose_n001(void)
{
    int rtn = (int)0xdeadbeef;
    rtn = TCSLibraryClose(INVALID_TCSLIB_HANDLE);
    dts_check_eq("TCSLibraryClose", rtn, -1, "TCSLibraryClose returned unexpected value.");
}


/**
 * @brief Negative test case of TCSLibraryOpen()
 */
static void utc_csr_framework_TCSLibraryOpen_n001(void)
{
    TCSLIB_HANDLE rtn = (TCSLIB_HANDLE)0xdeadbeef;
    rtn = TCSLibraryOpen();
    dts_check_eq("TCSLibraryOpen", rtn, INVALID_TCSLIB_HANDLE, "TCSLibraryOpen returned unexpected value.");
}


/**
 * @brief Negative test case of TCSScanData()
 */
static void utc_csr_framework_TCSScanData_n001(void)
{
    int rtn = (int)0xdeadbeef;
    rtn = TCSScanData(INVALID_TCSLIB_HANDLE,NULL,NULL);
    dts_check_eq("TCSScanData", rtn, -1, "TCSScanData returned unexpected value.");
}


/**
 * @brief Negative test case of TCSScanFile()
 */
static void utc_csr_framework_TCSScanFile_n001(void)
{
    int rtn = (int)0xdeadbeef;
    rtn = TCSScanFile(INVALID_TCSLIB_HANDLE,NULL,0,0,0,NULL);
    dts_check_eq("TCSScanFile", rtn, -1, "TCSScanFile returned unexpected value.");
}


/**
 * @brief Negative test case of TWPConfigurationCreate()
 */
static void utc_csr_framework_TWPConfigurationCreate_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPConfigurationCreate(INVALID_TWPLIB_HANDLE,NULL,NULL);
    dts_check_eq("TWPConfigurationCreate", rtn, TWP_NOT_IMPLEMENTED, "TWPConfigurationCreate returned unexpected value.");
}


/**
 * @brief Negative test case of TWPConfigurationDestroy()
 */
static void utc_csr_framework_TWPConfigurationDestroy_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPConfigurationDestroy(INVALID_TWPLIB_HANDLE,NULL);
    dts_check_eq("TWPConfigurationDestroy", rtn, TWP_NOT_IMPLEMENTED, "TWPConfigurationDestroy returned unexpected value.");
}


/**
 * @brief Negative test case of TWPInitLibrary()
 */
static void utc_csr_framework_TWPInitLibrary_n001(void)
{
    TWPLIB_HANDLE rtn = (TWPLIB_HANDLE)0xdeadbeef;
    rtn = TWPInitLibrary(NULL);
    dts_check_eq("TWPInitLibrary", rtn, INVALID_TWPLIB_HANDLE, "TWPInitLibrary returned unexpected value.");
}


/**
 * @brief Negative test case of TWPLookupUrls()
 */
static void utc_csr_framework_TWPLookupUrls_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPLookupUrls(INVALID_TWPLIB_HANDLE,NULL,NULL,0,NULL,0,NULL);
    dts_check_eq("TWPLookupUrls", rtn, TWP_NOT_IMPLEMENTED, "TWPLookupUrls returned unexpected value.");
}


/**
 * @brief Negative test case of TWPPolicyCreate()
 */
static void utc_csr_framework_TWPPolicyCreate_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPPolicyCreate(INVALID_TWPLIB_HANDLE,NULL,NULL,0,NULL);
    dts_check_eq("TWPPolicyCreate", rtn, TWP_NOT_IMPLEMENTED, "TWPPolicyCreate returned unexpected value.");
}


/**
 * @brief Negative test case of TWPPolicyDestroy()
 */
static void utc_csr_framework_TWPPolicyDestroy_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPPolicyDestroy(INVALID_TWPLIB_HANDLE,NULL);
    dts_check_eq("TWPPolicyDestroy", rtn, TWP_NOT_IMPLEMENTED, "TWPPolicyDestroy returned unexpected value.");
}


/**
 * @brief Negative test case of TWPPolicyGetViolations()
 */
static void utc_csr_framework_TWPPolicyGetViolations_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPPolicyGetViolations(INVALID_TWPLIB_HANDLE,NULL,NULL,NULL,NULL);
    dts_check_eq("TWPPolicyGetViolations", rtn, TWP_NOT_IMPLEMENTED, "TWPPolicyGetViolations returned unexpected value.");
}


/**
 * @brief Negative test case of TWPPolicyValidate()
 */
static void utc_csr_framework_TWPPolicyValidate_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPPolicyValidate(INVALID_TWPLIB_HANDLE,NULL,NULL,NULL);
    dts_check_eq("TWPPolicyValidate", rtn, TWP_NOT_IMPLEMENTED, "TWPPolicyValidate returned unexpected value.");
}


/**
 * @brief Negative test case of TWPResponseDestroy()
 */
static void utc_csr_framework_TWPResponseDestroy_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPResponseDestroy(INVALID_TWPLIB_HANDLE,NULL);
    dts_check_eq("TWPResponseDestroy", rtn, TWP_NOT_IMPLEMENTED, "TWPResponseDestroy returned unexpected value.");
}


/**
 * @brief Negative test case of TWPResponseGetRedirUrlFor()
 */
static void utc_csr_framework_TWPResponseGetRedirUrlFor_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPResponseGetRedirUrlFor(INVALID_TWPLIB_HANDLE,NULL,NULL,NULL,NULL,NULL);
    dts_check_eq("TWPResponseGetRedirUrlFor", rtn, TWP_NOT_IMPLEMENTED, "TWPResponseGetRedirUrlFor returned unexpected value.");
}


/**
 * @brief Negative test case of TWPResponseGetUrlRatingByIndex()
 */
static void utc_csr_framework_TWPResponseGetUrlRatingByIndex_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPResponseGetUrlRatingByIndex(INVALID_TWPLIB_HANDLE,NULL,0,NULL);
    dts_check_eq("TWPResponseGetUrlRatingByIndex", rtn, TWP_NOT_IMPLEMENTED, "TWPResponseGetUrlRatingByIndex returned unexpected value.");
}


/**
 * @brief Negative test case of TWPResponseGetUrlRatingByUrl()
 */
static void utc_csr_framework_TWPResponseGetUrlRatingByUrl_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPResponseGetUrlRatingByUrl(INVALID_TWPLIB_HANDLE,NULL,NULL,0,NULL);
    dts_check_eq("TWPResponseGetUrlRatingByUrl", rtn, TWP_NOT_IMPLEMENTED, "TWPResponseGetUrlRatingByUrl returned unexpected value.");
}


/**
 * @brief Negative test case of TWPResponseGetUrlRatingsCount()
 */
static void utc_csr_framework_TWPResponseGetUrlRatingsCount_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPResponseGetUrlRatingsCount(INVALID_TWPLIB_HANDLE,NULL,NULL);
    dts_check_eq("TWPResponseGetUrlRatingsCount", rtn, TWP_NOT_IMPLEMENTED, "TWPResponseGetUrlRatingsCount returned unexpected value.");
}


/**
 * @brief Negative test case of TWPResponseWrite()
 */
static void utc_csr_framework_TWPResponseWrite_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPResponseWrite(INVALID_TWPLIB_HANDLE,NULL,NULL,0);
    dts_check_eq("TWPResponseWrite", rtn, TWP_NOT_IMPLEMENTED, "TWPResponseWrite returned unexpected value.");
}


/**
 * @brief Negative test case of TWPUninitLibrary()
 */
static void utc_csr_framework_TWPUninitLibrary_n001(void)
{
    TWPUninitLibrary(INVALID_TWPLIB_HANDLE);
    dts_pass("TWPUninitLibrary", "TWPUninitLibrary passed as expected");
}


/**
 * @brief Negative test case of TWPUrlRatingGetCategories()
 */
static void utc_csr_framework_TWPUrlRatingGetCategories_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPUrlRatingGetCategories(INVALID_TWPLIB_HANDLE,NULL,NULL,NULL);
    dts_check_eq("TWPUrlRatingGetCategories", rtn, TWP_NOT_IMPLEMENTED, "TWPUrlRatingGetCategories returned unexpected value.");
}


/**
 * @brief Negative test case of TWPUrlRatingGetDLAUrl()
 */
static void utc_csr_framework_TWPUrlRatingGetDLAUrl_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPUrlRatingGetDLAUrl(INVALID_TWPLIB_HANDLE,NULL,NULL,NULL);
    dts_check_eq("TWPUrlRatingGetDLAUrl", rtn, TWP_NOT_IMPLEMENTED, "TWPUrlRatingGetDLAUrl returned unexpected value.");
}


/**
 * @brief Negative test case of TWPUrlRatingGetScore()
 */
static void utc_csr_framework_TWPUrlRatingGetScore_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPUrlRatingGetScore(INVALID_TWPLIB_HANDLE,NULL,NULL);
    dts_check_eq("TWPUrlRatingGetScore", rtn, TWP_NOT_IMPLEMENTED, "TWPUrlRatingGetScore returned unexpected value.");
}


/**
 * @brief Negative test case of TWPUrlRatingGetUrl()
 */
static void utc_csr_framework_TWPUrlRatingGetUrl_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPUrlRatingGetUrl(INVALID_TWPLIB_HANDLE,NULL,NULL,NULL);
    dts_check_eq("TWPUrlRatingGetUrl", rtn, TWP_NOT_IMPLEMENTED, "TWPUrlRatingGetUrl returned unexpected value.");
}


/**
 * @brief Negative test case of TWPUrlRatingHasCategory()
 */
static void utc_csr_framework_TWPUrlRatingHasCategory_n001(void)
{
    TWP_RESULT rtn = (TWP_RESULT)0xdeadbeef;
    rtn = TWPUrlRatingHasCategory(INVALID_TWPLIB_HANDLE,NULL,TWP_LastCategoryPlaceholder,NULL);
    dts_check_eq("TWPUrlRatingHasCategory", rtn, TWP_NOT_IMPLEMENTED, "TWPUrlRatingHasCategory returned unexpected value.");
}



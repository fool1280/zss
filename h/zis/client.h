

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
  
  SPDX-License-Identifier: EPL-2.0
  
  Copyright Contributors to the Zowe Project.
*/

#ifndef ZIS_CLIENT_H_
#define ZIS_CLIENT_H_

#include "zowetypes.h"
#include "crossmemory.h"
#include "zos.h"

#include "zis/server.h"
#include "zis/service.h"

#include "zis/services/auth.h"
#include "zis/services/nwm.h"
#include "zis/services/secmgmt.h"
#include "zis/services/snarfer.h"

#ifndef __LONGNAME__

#define zisCallService              ZISCSRVC
#define zisCallServiceUnchecked     ZISCUSVC
#define zisCallVersionedService     ZISCVSVC

#define zisGetDefaultServerName     ZISGDSNM
#define zisCopyDataFromAddressSpace ZISCOPYD
#define zisCheckUsernameAndPassword ZISUNPWD
#define zisCheckEntity              ZISSAUTH
#define zisGetSAFAccessLevel        ZISGETAC
#define zisCallNWMService           ZISCNWMS
#define zisExtractUserProfiles      ZISUPROF
#define zisExtractGenresProfiles    ZISGRROF
#define zisExtractGenresAccessList  ZISGRAST
#define zisDefineProfile            ZISDFPRF
#define zisDeleteProfile            ZISDLPRF
#define zisGiveAccessToProfile      ZISGAPRF
#define zisRevokeAccessToProfile    ZISRAPRF
#define zisExtractGroupProfiles     ZISXGRPP
#define zisExtractGroupAccessList   ZISXGRPA
#define zisAddGroup                 ZISADDGP
#define zisDeleteGroup              ZISDELGP
#define zisConnectToGroup           ZISADDCN
#define zisRemoveFromGroup          ZISREMCN
#define zisGenerateOrValidateSafIdtWithAppl ZISGVSAA
#define zisGenerateOrValidateSafIdt         ZISGVSAF

#endif

#define NOOP(...)

static const char *ZIS_COPY_RC_DESCRIPTION[] = {
        [RC_ZIS_SNRFSRV_OK] = "Ok",
        [RC_ZIS_SNRFSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_SNRFSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_SNRFSRV_BAD_DEST] = "Bad destination",
        [RC_ZIS_SNRFSRV_BAD_ASCB] = "Bad ASCB",
        [RC_ZIS_SNRFSRV_BAD_SIZE] = "Bad size",
        [RC_ZIS_SNRFSRV_ECSA_ALLOC_FAILED] = "ESCA allocation failed",
        [RC_ZIS_SNRFSRV_RECOVERY_ERROR] = "Recovery error",
        [RC_ZIS_SNRFSRV_SRC_ASSB_ABEND] = "ASSB abended",
        [RC_ZIS_SNRFSRV_SRC_IEAMSCHD_ABEND] = "IEAMSCHD abended",
        [RC_ZIS_SNRFSRV_SRC_IEAMSCHD_FAILED] = "IEAMSCHD failed",
        [RC_ZIS_SNRFSRV_SHARED_OBJ_ALLOC_FAILED] = "Shared object allocation failed",
        [RC_ZIS_SNRFSRV_SHARED_OBJ_SHARE_FAILED] = "Shared object sharing failed",
        [RC_ZIS_SNRFSRV_SHARED_OBJ_DETACH_FAILED] = "Shared object detach failed "
};

#define FORMAT_RC($rc, $arr) (\
  ($rc >= 0 && $rc < sizeof($arr) / sizeof($arr[0]))? \
      $arr[$rc] : "Unknown return code" \
)

typedef struct ZISServiceStatus_tag {
  int serviceRC;
  int serviceRSN;
  int cmsRC;
  int cmsRSN;
} ZISServiceStatus;

int zisCallService(const CrossMemoryServerName *serverName,
                   const ZISServicePath *path, void *parm,
                   ZISServiceStatus *status);

int zisCallServiceUnchecked(const CrossMemoryServerName *serverName,
                            const ZISServicePath *path, void *parm,
                            ZISServiceStatus *status);

int zisCallVersionedService(const CrossMemoryServerName *serverName,
                            const ZISServicePath *path, void *parm,
                            unsigned int version,
                            ZISServiceStatus *status);

#define _ZIS_FORMAT_CALL_STATUS_TMPL($rc, $s, $printf, $serviceCases,\
                                     $descriptions) do {\
  switch (($rc)) {\
  case RC_ZIS_SRVC_OK:\
    $printf("Call successful");\
    break;\
  case RC_ZIS_SRVC_CMS_FAILED:\
    $printf("Problem with the cross-memory call, RC %d", ($s)->baseStatus.cmsRC);\
    break;\
  case RC_ZIS_SRVC_SERVICE_FAILED:\
    switch (($s)->baseStatus.serviceRC) {\
    $serviceCases($rc, $s, $printf, $descriptions)\
    default:\
      $printf("Service error: %s", FORMAT_RC(($s)->baseStatus.serviceRC, \
              $descriptions));\
      break;\
    }\
    break;\
  default:\
    $printf("Unknown error code %d", ($rc));\
    break;\
  }\
} while (0)

CrossMemoryServerName zisGetDefaultServerName(void);

typedef struct ZISCopyServiceStatus_tag {
  ZISServiceStatus baseStatus;
} ZISCopyServiceStatus;

#define ZIS_FORMAT_COPY_CALL_STATUS($rc, $s, $printf) \
    _ZIS_FORMAT_CALL_STATUS_TMPL($rc, $s, $printf, NOOP, \
                                 ZIS_COPY_RC_DESCRIPTION)

int zisCopyDataFromAddressSpace(const CrossMemoryServerName *serverName,
                                void *dest, void *src, unsigned int size,
                                int srcKey, ASCB *ascb,
                                ZISCopyServiceStatus *result);

static const char *ZIS_AUTH_RC_DESCRIPTION[] = {
        [RC_ZIS_AUTHSRV_OK] = "Ok",
        [RC_ZIS_AUTHSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_AUTHSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_AUTHSRV_DELETE_FAILED] = "Deleting ACEE failed",
        [RC_ZIS_AUTHSRV_CREATE_FAILED] = "Creating ACEE failed",
        [RC_ZIS_AUTHSRV_UNKNOWN_FUNCTION_CODE] = "Unknown function code",
        [RC_ZIS_AUTHSRV_INPUT_STRING_TOO_LONG] = "Input string too long",
        [RC_ZIS_AUTHSRV_INSTALL_RECOVERY_FAILED] = "Installing recovery failed",
        [RC_ZIS_AUTHSRV_SAF_ABENDED] = "SAF abended",
        [RC_ZIS_AUTHSRV_SAF_ERROR] = "SAF error",
        [RC_ZIS_AUTHSRV_SAF_NO_DECISION] = "No SAF decision",
        [RC_ZIS_AUTHSRV_USER_CLASS_NOT_READ] = CMS_PROD_ID" class read failed",
        [RC_ZIS_AUTHSRV_USER_CLASS_TOO_LONG] = CMS_PROD_ID" class is too long",
};

typedef struct ZISAuthServiceStatus_tag {
  ZISServiceStatus baseStatus;
  union {
    SAFAuthStatus safStatus;
    AbendInfo abendInfo;
  };
} ZISAuthServiceStatus;

#define _ZIS_AUTH_SERVICE_ERROR_CASES($rc, $status, $printf, $descriptions) \
  case RC_ZIS_AUTHSRV_SAF_ERROR:\
    FORMAT_SAF_STATUS((&($status)->safStatus), $printf);\
    break;\
  case RC_ZIS_AUTHSRV_SAF_ABENDED:\
    FORMAT_ABEND_INFO((&($status)->abendInfo), $printf);\
    break;\


#define ZIS_FORMAT_AUTH_CALL_STATUS($rc, $status, $printf) \
    _ZIS_FORMAT_CALL_STATUS_TMPL($rc, $status, $printf, \
        _ZIS_AUTH_SERVICE_ERROR_CASES, ZIS_AUTH_RC_DESCRIPTION)

int zisCheckUsernameAndPassword(const CrossMemoryServerName *serverName,
                                const char *userName, const char *password,
                                ZISAuthServiceStatus *status);

int zisCheckEntity(const CrossMemoryServerName *serverName,
                   const char *userName, const char *class, const char *entity,
                   int access, ZISAuthServiceStatus *status);

/* The following enum values must be in sync with RACF RSN in RACROUTE
 * REQUEST=AUTH with SAF RC = 0 and RACF RC = 14 */
typedef enum ZISSAFAccessLevel_tag {
  ZIS_SAF_ACCESS_NONE = 0x00,
  ZIS_SAF_ACCESS_READ = 0x04,
  ZIS_SAF_ACCESS_UPDATE  = 0x08,
  ZIS_SAF_ACCESS_CONTROL = 0x0C,
  ZIS_SAF_ACCESS_ALETER = 0x10,
} ZISSAFAccessLevel;

int zisGetSAFAccessLevel(const CrossMemoryServerName *serverName,
                         const char *id,
                         const char *class,
                         const char *entity,
                         ZISSAFAccessLevel *access,
                         ZISAuthServiceStatus *status,
                         int traceLevel);

typedef struct ZISUserProfileServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
} ZISUserProfileServiceStatus;

int zisExtractUserProfiles(const CrossMemoryServerName *serverName,
                           const char *startProfile,
                           unsigned int profilesToExtract,
                           ZISUserProfileEntry *resultBuffer,
                           unsigned int *profilesExtracted,
                           ZISUserProfileServiceStatus *status,
                           int traceLevel);

static const char *ZIS_UPRFSRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_UPRFSRV_OK] = "Ok",
        [RC_ZIS_UPRFSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_UPRFSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_UPRFSRV_USER_ID_TOO_LONG] = "User ID is too long",
        [RC_ZIS_UPRFSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_UPRFSRV_PROFILE_COUNT_NULL] = "Profile count is null",
        [RC_ZIS_UPRFSRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_UPRFSRV_INTERNAL_SERVICE_FAILED] = "R_admin service failed",
        [RC_ZIS_UPRFSRV_ALLOC_FAILED] = "Alloc function failed",
        [RC_ZIS_UPRFSRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_UPRFSRV_USER_ID_TOO_LONG        (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_UPRFSRV_RESULT_BUFF_NULL        (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_UPRFSRV_PROFILE_COUNT_NULL      (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_UPRFSRV_MAX_RC                  (ZIS_MAX_GEN_SRVC_RC + 3)

static const char *ZIS_UPRFSRV_WRAPPER_RC_DESCRIPTION[] = {
    [RC_ZIS_SRVC_UPRFSRV_USER_ID_TOO_LONG] = "User ID is too long",
    [RC_ZIS_SRVC_UPRFSRV_RESULT_BUFF_NULL] = "Result buffer is null",
    [RC_ZIS_SRVC_UPRFSRV_PROFILE_COUNT_NULL] = "Profile count is null"
};

typedef struct ZISGenresProfileServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
} ZISGenresProfileServiceStatus;

int zisExtractGenresProfiles(const CrossMemoryServerName *serverName,
                             const char *class,
                             const char *startProfile,
                             unsigned int profilesToExtract,
                             ZISGenresProfileEntry *resultBuffer,
                             unsigned int *profilesExtracted,
                             ZISGenresProfileServiceStatus *status,
                             int traceLevel);

static const char *ZIS_GRPRFSRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_GRPRFSRV_OK] = "Ok",
        [RC_ZIS_GRPRFSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_GRPRFSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_GRPRFSRV_CLASS_TOO_LONG] = "Class is too long",
        [RC_ZIS_GRPRFSRV_PROFILE_TOO_LONG] = "Profile is too long",
        [RC_ZIS_GRPRFSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_GRPRFSRV_PROFILE_COUNT_NULL] = "Profile count is null",
        [RC_ZIS_GRPRFSRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_GRPRFSRV_INTERNAL_SERVICE_FAILED] = "R_admin failed",
        [RC_ZIS_GRPRFSRV_ALLOC_FAILED] = "Alloc function failed",
        [RC_ZIS_GRPRFSRV_USER_CLASS_NOT_READ] = CMS_PROD_ID" class is not read",
        [RC_ZIS_GRPRFSRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_GRPRFSRV_CLASS_TOO_LONG        (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_GRPRFSRV_PROFILE_TOO_LONG      (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_GRPRFSRV_RESULT_BUFF_NULL      (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_GRPRFSRV_PROFILE_COUNT_NULL    (ZIS_MAX_GEN_SRVC_RC + 4)
#define RC_ZIS_SRVC_GRPRFSRV_MAX_RC                (ZIS_MAX_GEN_SRVC_RC + 4)

static const char *ZIS_GRPRFSRV_WRAPPER_RC_DESCRIPTION[] = {
    [RC_ZIS_SRVC_GRPRFSRV_CLASS_TOO_LONG] = "Class is too long",
    [RC_ZIS_SRVC_GRPRFSRV_PROFILE_TOO_LONG] = "Profile is too long",
    [RC_ZIS_SRVC_GRPRFSRV_RESULT_BUFF_NULL] = "Result buffer is null",
    [RC_ZIS_SRVC_GRPRFSRV_PROFILE_COUNT_NULL] = "Profile count is null"
};

typedef struct ZISGenresAccessListServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
} ZISGenresAccessListServiceStatus;

int zisExtractGenresAccessList(const CrossMemoryServerName *serverName,
                               const char *class,
                               const char *profile,
                               ZISGenresAccessEntry *resultBuffer,
                               unsigned int resultBufferCapacity,
                               unsigned int *entriesExtracted,
                               ZISGenresAccessListServiceStatus *status,
                               int traceLevel);

static const char *ZIS_ACSLSRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_ACSLSRV_OK] = "Ok",
        [RC_ZIS_ACSLSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_ACSLSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_ACSLSRV_CLASS_TOO_LONG] = "Class is too long",
        [RC_ZIS_ACSLSRV_PROFILE_TOO_LONG] = "Profile is too long",
        [RC_ZIS_ACSLSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_ACSLSRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_ACSLSRV_ALLOC_FAILED] = "Alloc function failed",
        [RC_ZIS_ACSLSRV_INTERNAL_SERVICE_FAILED] = "R_admin failed",
        [RC_ZIS_ACSLSRV_INSUFFICIENT_SPACE] = "Provided buffer is too small",
        [RC_ZIS_ACSLSRV_USER_CLASS_NOT_READ] = CMS_PROD_ID" class is not read",
        [RC_ZIS_ACSLSRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_ACSLSRV_CLASS_TOO_LONG          (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_ACSLSRV_PROFILE_NULL            (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_ACSLSRV_PROFILE_TOO_LONG        (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_ACSLSRV_RESULT_BUFF_NULL        (ZIS_MAX_GEN_SRVC_RC + 4)
#define RC_ZIS_SRVC_ACSLSRV_ENTRY_COUNT_NULL        (ZIS_MAX_GEN_SRVC_RC + 5)
#define RC_ZIS_SRVC_ACSLSRV_INSUFFICIENT_BUFFER     (ZIS_MAX_GEN_SRVC_RC + 6)
#define RC_ZIS_SRVC_ACSLSRV_MAX_RC                  (ZIS_MAX_GEN_SRVC_RC + 6)

static const char *ZIS_ACSLSRV_WRAPPER_RC_DESCRIPTION[] = {
        [RC_ZIS_SRVC_ACSLSRV_CLASS_TOO_LONG] = "Class is too long",
        [RC_ZIS_SRVC_ACSLSRV_PROFILE_NULL] = "Profile is null",
        [RC_ZIS_SRVC_ACSLSRV_PROFILE_TOO_LONG] = "Profile is too long",
        [RC_ZIS_SRVC_ACSLSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_SRVC_ACSLSRV_ENTRY_COUNT_NULL] = "Entry count is null",
        [RC_ZIS_SRVC_ACSLSRV_INSUFFICIENT_BUFFER] = "Provided buffer is too small"
};

typedef struct ZISGenresAdminServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
  ZISGenresAdminServiceMessage message;
} ZISGenresAdminServiceStatus;

int zisDefineProfile(const CrossMemoryServerName *serverName,
                     const char *profileName,
                     const char *owner,
                     bool isDryRun,
                     ZISGenresAdminServiceMessage *operatorCommand,
                     ZISGenresAdminServiceStatus *status,
                     int traceLevel);

int zisDeleteProfile(const CrossMemoryServerName *serverName,
                     const char *profileName,
                     bool isDryRun,
                     ZISGenresAdminServiceMessage *operatorCommand,
                     ZISGenresAdminServiceStatus *status,
                     int traceLevel);

int zisGiveAccessToProfile(const CrossMemoryServerName *serverName,
                           const char *userID,
                           const char *profileName,
                           ZISGenresAdminServiceAccessType accessType,
                           bool isDryRun,
                           ZISGenresAdminServiceMessage *operatorCommand,
                           ZISGenresAdminServiceStatus *status,
                           int traceLevel);

int zisRevokeAccessToProfile(const CrossMemoryServerName *serverName,
                             const char *userID,
                             const char *profileName,
                             bool isDryRun,
                             ZISGenresAdminServiceMessage *operatorCommand,
                             ZISGenresAdminServiceStatus *status,
                             int traceLevel);

static const char *ZIS_GSADMNSRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_GSADMNSRV_OK] = "Ok",
        [RC_ZIS_GSADMNSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_GSADMNSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_GSADMNSRV_PROF_TOO_LONG] = "Profile is too long",
        [RC_ZIS_GSADMNSRV_UNSUPPORTED_PRODUCT] = "Unsupported product",
        [RC_ZIS_GSADMNSRV_USER_ID_NULL] = "User ID is null",
        [RC_ZIS_GSADMNSRV_USER_ID_TOO_LONG] = "User ID is too long",
        [RC_ZIS_GSADMNSRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_GSADMNSRV_BAD_FUNCTION] = "Bad function",
        [RC_ZIS_GSADMNSRV_BAD_ACCESS_TYPE] = "Bad access type",
        [RC_ZIS_GSADMNSRV_INTERNAL_SERVICE_FAILED] = "R_admin failed",
        [RC_ZIS_GSADMNSRV_PARM_INTERNAL_ERROR] = "Parm list creation failed",
        [RC_ZIS_GSADMNSRV_USER_CLASS_TOO_LONG] = CMS_PROD_ID" class is too long",
        [RC_ZIS_GSADMNSRV_USER_CLASS_NOT_READ] = CMS_PROD_ID" class read failed",
        [RC_ZIS_GSADMNSRV_AUTOREFRESH_NOT_READ] = "Auto refresh option read failed",
        [RC_ZIS_GSADMNSRV_OWNER_TOO_LONG] = "Owner name is too long",
        [RC_ZIS_GSADMNSRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_PADMIN_USERID_NULL            (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_PADMIN_USERID_TOO_LONG        (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_PADMIN_PROFILE_NULL           (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_PADMIN_PROFILE_TOO_LONG       (ZIS_MAX_GEN_SRVC_RC + 4)
#define RC_ZIS_SRVC_PADMIN_OWNER_NULL             (ZIS_MAX_GEN_SRVC_RC + 5)
#define RC_ZIS_SRVC_PADMIN_OWNER_TOO_LONG         (ZIS_MAX_GEN_SRVC_RC + 6)
#define RC_ZIS_SRVC_PADMIN_OPER_CMD_NULL          (ZIS_MAX_GEN_SRVC_RC + 7)
#define RC_ZIS_SRVC_PADMIN_MAX_RC                 (ZIS_MAX_GEN_SRVC_RC + 7)

static const char *ZIS_GSADMNSRV_WRAPPER_RC_DESCRIPTION[] = {
        [RC_ZIS_SRVC_PADMIN_USERID_NULL] = "User ID is null",
        [RC_ZIS_SRVC_PADMIN_USERID_TOO_LONG] = "User ID is too long",
        [RC_ZIS_SRVC_PADMIN_PROFILE_NULL] = "Profile is null",
        [RC_ZIS_SRVC_PADMIN_PROFILE_TOO_LONG] = "Profile is too long",
        [RC_ZIS_SRVC_PADMIN_OPER_CMD_NULL] = "Operator command is null"
};

typedef struct ZISGroupProfileServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
} ZISGroupProfileServiceStatus;

int zisExtractGroupProfiles(const CrossMemoryServerName *serverName,
                            const char *startGroup,
                            unsigned int profilesToExtract,
                            ZISGroupProfileEntry *resultBuffer,
                            unsigned int *profilesExtracted,
                            ZISGroupProfileServiceStatus *status,
                            int traceLevel);

static const char *ZIS_GPPRFSRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_GPPRFSRV_OK] = "Ok",
        [RC_ZIS_GPPRFSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_GPPRFSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_GPPRFSRV_GROUP_TOO_LONG] = "Group is too long",
        [RC_ZIS_GPPRFSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_GPPRFSRV_PROFILE_COUNT_NULL] = "Profile count is null",
        [RC_ZIS_GPPRFSRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_GPPRFSRV_INTERNAL_SERVICE_FAILED] = "R_admin failed",
        [RC_ZIS_GPPRFSRV_ALLOC_FAILED] = "Alloc function failed",
        [RC_ZIS_GPPRFSRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_GPPRFSRV_CLASS_TOO_LONG         (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_GPPRFSRV_GROUP_TOO_LONG         (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_GPPRFSRV_RESULT_BUFF_NULL       (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_GPPRFSRV_PROFILE_COUNT_NULL     (ZIS_MAX_GEN_SRVC_RC + 4)
#define RC_ZIS_SRVC_GPPRFSRV_MAX_RC                 (ZIS_MAX_GEN_SRVC_RC + 4)

static const char *ZIS_GPPRFSRV_WRAPPER_RC_DESCRIPTION[] = {
        [RC_ZIS_SRVC_GPPRFSRV_CLASS_TOO_LONG] = "Class is too long",
        [RC_ZIS_SRVC_GPPRFSRV_GROUP_TOO_LONG] = "Group is too long",
        [RC_ZIS_SRVC_GPPRFSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_SRVC_GPPRFSRV_PROFILE_COUNT_NULL] = "Profile count is null"
};

typedef struct ZISGroupAccessListServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
} ZISGroupAccessListServiceStatus;

int zisExtractGroupAccessList(const CrossMemoryServerName *serverName,
                              const char *group,
                              ZISGroupAccessEntry *resultBuffer,
                              unsigned int resultBufferCapacity,
                              unsigned int *entriesExtracted,
                              ZISGroupAccessListServiceStatus *status,
                              int traceLevel);

static const char *ZIS_GRPALSRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_GRPALSRV_OK] = "Ok",
        [RC_ZIS_GRPALSRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_GRPALSRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_GRPALSRV_GROUP_TOO_LONG] = "Group is too long",
        [RC_ZIS_GRPALSRV_RESULT_BUFF_NULL] = "Result buffer is null",
        [RC_ZIS_GRPALSRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_GRPALSRV_ALLOC_FAILED] = "Alloc function failed",
        [RC_ZIS_GRPALSRV_INTERNAL_SERVICE_FAILED] = "R_admin failed",
        [RC_ZIS_GRPALSRV_INSUFFICIENT_SPACE] = "Provided buffer is too small",
        [RC_ZIS_GRPALSRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_GRPALSRV_GROUP_NULL             (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_GRPALSRV_GROUP_TOO_LONG         (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_GRPALSRV_RESULT_BUFF_NULL       (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_GRPALSRV_ENTRY_COUNT_NULL       (ZIS_MAX_GEN_SRVC_RC + 4)
#define RC_ZIS_SRVC_GRPALSRV_INSUFFICIENT_BUFFER    (ZIS_MAX_GEN_SRVC_RC + 5)
#define RC_ZIS_SRVC_GRPALSRV_MAX_RC                 (ZIS_MAX_GEN_SRVC_RC + 6)

static const char *ZIS_GRPALSRV_WRAPPER_RC_DESCRIPTION[] = {
    [RC_ZIS_SRVC_GRPALSRV_GROUP_NULL] = "Group is null",
    [RC_ZIS_SRVC_GRPALSRV_GROUP_TOO_LONG] = "Group is too long",
    [RC_ZIS_SRVC_GRPALSRV_RESULT_BUFF_NULL] = "Result buffer is null",
    [RC_ZIS_SRVC_GRPALSRV_ENTRY_COUNT_NULL] = "Entry count is null",
    [RC_ZIS_SRVC_GRPALSRV_INSUFFICIENT_BUFFER] = "Provided buffer is too small"
};

typedef struct ZISGroupAdminServiceStatus_tag {
  ZISServiceStatus baseStatus;
  int internalServiceRC;
  int internalServiceRSN;
  SAFStatus safStatus;
  ZISGroupAdminServiceMessage message;
} ZISGroupAdminServiceStatus;

int zisAddGroup(const CrossMemoryServerName *serverName,
                const char *groupName,
                const char *superiorGroupName,
                bool isDryRun,
                ZISGroupAdminServiceMessage *operatorCommand,
                ZISGroupAdminServiceStatus *status,
                int traceLevel);

int zisDeleteGroup(const CrossMemoryServerName *serverName,
                   const char *groupName,
                   bool isDryRun,
                   ZISGroupAdminServiceMessage *operatorCommand,
                   ZISGroupAdminServiceStatus *status,
                   int traceLevel);

int zisConnectToGroup(const CrossMemoryServerName *serverName,
                      const char *userID,
                      const char *groupName,
                      ZISGroupAdminServiceAccessType accessType,
                      bool isDryRun,
                      ZISGroupAdminServiceMessage *operatorCommand,
                      ZISGroupAdminServiceStatus *status,
                      int traceLevel);

int zisRemoveFromGroup(const CrossMemoryServerName *serverName,
                       const char *userID,
                       const char *groupName,
                       bool isDryRun,
                       ZISGroupAdminServiceMessage *operatorCommand,
                       ZISGroupAdminServiceStatus *status,
                       int traceLevel);

static const char *ZIS_GRPASRV_SERVICE_RC_DESCRIPTION[] = {
        [RC_ZIS_GRPASRV_OK] = "Ok",
        [RC_ZIS_GRPASRV_PARMLIST_NULL] = "Parm list is null",
        [RC_ZIS_GRPASRV_BAD_EYECATCHER] = "Bad eyecatcher",
        [RC_ZIS_GRPASRV_GROUP_TOO_LONG] = "Group is too long",
        [RC_ZIS_GRPASRV_SUPGROUP_TOO_LONG] = "Superior group is too long",
        [RC_ZIS_GRPASRV_UNSUPPORTED_PRODUCT] = "Unsupported product",
        [RC_ZIS_GRPASRV_USER_ID_TOO_LONG] = "User ID is too long",
        [RC_ZIS_GRPASRV_IMPERSONATION_MISSING] = "Impersonation is required",
        [RC_ZIS_GRPASRV_BAD_FUNCTION] = "Bad function",
        [RC_ZIS_GRPASRV_BAD_ACCESS_TYPE] = "Bad access type",
        [RC_ZIS_GRPASRV_INTERNAL_SERVICE_FAILED] = "R_admin failed",
        [RC_ZIS_GRPASRV_PARM_INTERNAL_ERROR] = "Parm list creation failed",
        [RC_ZIS_GRPASRV_USER_CLASS_TOO_LONG] = CMS_PROD_ID" class is too long",
        [RC_ZIS_GRPASRV_USER_CLASS_NOT_READ] = CMS_PROD_ID" class read failed",
        [RC_ZIS_GRPASRV_AUTOREFRESH_NOT_READ] = "Auto refresh option read failed",
        [RC_ZIS_GRPASRV_UNSUPPORTED_ESM] = "ESM not supported"
};

#define RC_ZIS_SRVC_GADMIN_GROUP_NULL             (ZIS_MAX_GEN_SRVC_RC + 1)
#define RC_ZIS_SRVC_GADMIN_GROUP_TOO_LONG         (ZIS_MAX_GEN_SRVC_RC + 2)
#define RC_ZIS_SRVC_GADMIN_SUPGROUP_NULL          (ZIS_MAX_GEN_SRVC_RC + 3)
#define RC_ZIS_SRVC_GADMIN_SUPGROUP_TOO_LONG      (ZIS_MAX_GEN_SRVC_RC + 4)
#define RC_ZIS_SRVC_GADMIN_USERID_NULL            (ZIS_MAX_GEN_SRVC_RC + 5)
#define RC_ZIS_SRVC_GADMIN_USERID_TOO_LONG        (ZIS_MAX_GEN_SRVC_RC + 6)
#define RC_ZIS_SRVC_GADMIN_OPER_CMD_NULL          (ZIS_MAX_GEN_SRVC_RC + 7)
#define RC_ZIS_SRVC_GADMIN_MAX_RC                 (ZIS_MAX_GEN_SRVC_RC + 7)

static const char *ZIS_GRPASRV_WRAPPER_RC_DESCRIPTION[] = {
        [RC_ZIS_SRVC_GADMIN_GROUP_NULL] = "Group is null",
        [RC_ZIS_SRVC_GADMIN_GROUP_TOO_LONG] = "Group is too long",
        [RC_ZIS_SRVC_GADMIN_SUPGROUP_NULL] = "Superior group is null",
        [RC_ZIS_SRVC_GADMIN_SUPGROUP_TOO_LONG] = "Superior group is too long",
        [RC_ZIS_SRVC_GADMIN_OPER_CMD_NULL] = "Operator command is null"
};

typedef struct ZISNWMJobName_tag {
  char value[8];
} ZISNWMJobName;

typedef struct ZISNWMService_tag {
  ZISServiceStatus baseStatus;
  int nmiReturnValue;
  int nmiReturnCode;
  int nmiReasonCode;
} ZISNWMServiceStatus;

int zisCallNWMService(const CrossMemoryServerName *serverName,
                      ZISNWMJobName jobName,
                      char *nmiBuffer,
                      unsigned int nmiBufferSize,
                      ZISNWMServiceStatus *status,
                      int traceLevel);

#define RC_ZIS_SRVC_NWM_BUFFER_NULL               (ZIS_MAX_GEN_SRVC_RC + 1)

/**
 * @brief This function generates the SAF ID token when username and password is provided and SAF ID token is empty.
 * This function validates the SAF ID token for username when token is provided and password is empty. Supports only
 * JWT as identity token. Supports application name as audience claim. Returns RC_ZIS_AUTHSRV_SAF_ERROR when username,
 * password and SAF ID token is empty.
 *
 * @param serverName Cross-memory server that validates the parameters
 * @param userName Name of the user to whom the token belongs
 * @param password Password of the user
 * @param appl Application name parameter to specify the audience claim for SAF IDT. It is optional parameter. When
 * provided for generation the token, this will be included in the token. When provided for validation, it will be
 * validated against audience claim.
 * @param safIdt Token that will be validated.
 * @param status Status code from ZIS.
 *
 * @return One of the RC_ZIS_AUTHSRV_xx return codes.
 */
int zisGenerateOrValidateSafIdtWithAppl(const CrossMemoryServerName *serverName,
                                const char *userName, const char *password,
                                const char *appl,
                                char *safIdt,
                                ZISAuthServiceStatus *status);

/**
 * @brief This function generates the SAF ID token when username and password is provided and SAF ID token is empty.
 * This function validates the SAF ID token for username when token is provided and password is empty. Supports only
 * JWT as identity token.Returns RC_ZIS_AUTHSRV_SAF_ERROR when username, password and SAF ID token is empty.
 *
 * @param serverName Cross-memory server that validates the parameters
 * @param userName Name of the user to whom the token belongs
 * @param password Password of the user
 * @param safIdt Token that will be validated.
 * @param status Status code from ZIS.
 *
 * @return One of the RC_ZIS_AUTHSRV_xx return codes.
 */
int zisGenerateOrValidateSafIdt(const CrossMemoryServerName *serverName,
                                const char *userName, const char *password,
                                char *safIdt,
                                ZISAuthServiceStatus *status);

#endif /* ZIS_CLIENT_H_ */



/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
  
  SPDX-License-Identifier: EPL-2.0
  
  Copyright Contributors to the Zowe Project.
*/


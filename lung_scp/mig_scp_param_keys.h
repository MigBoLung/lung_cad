#ifndef __SCP_PARAM_KEYS_H__
#define __SCP_PARAM_KEYS_H__

#define PARAM_SCP_PORT          "network:dicom_port"
#define PARAM_SCP_MAX_PDU       "network:max_pdu"

#define PARAM_SCP_ACSE_TIMEOUT  "network:acse_timeout"
#define PARAM_SCP_DIMSE_TIMEOUT  "network:dimse_timeout"

#define PARAM_SCP_CALLING_AE    "network:calling_aetitle"
#define PARAM_SCP_CALLED_AE     "network:called_aetitle"

#define PARAM_SCP_AS_CONFIG     "association:config"
#define PARAM_SCP_PROFILE       "LUNGSCP"

#define PARAM_SCP_DIR_BASE      "storage:dir_base"
#define PARAM_SCP_DB_FILE       "storage:db_file"

#define SCP_LOGGER_NAME		"scp_logger"

/* defualt scp parameters */
#define DEFAULT_SCP_PORT        4006
#define DEFAULT_SCP_MAX_PDU     30000

#define DEFAULT_SCP_ACSE_TIMEOUT    0
#define DEFAULT_SCP_DIMSE_TIMEOUT   0

#define DEFAULT_SCP_AETITLE_CALLING     "LUNGSTORESCU"
#define DEFAULT_SCP_AETITLE_CALLED      "LUNGSTORESCP"

#endif /* __SCP_PARAM_KEYS_H__ */


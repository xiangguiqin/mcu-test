#ifndef GSV2K11_VSP_MAP_FCT_H
#define GSV2K11_VSP_MAP_FCT_H
#define GSV2K11_VSP_get_DG_VID(port, pval)                             AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x00, 0xFF, 0, pval)
#define GSV2K11_VSP_set_DG_VID(port, val)                              AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x00, 0xFF, 0, val)
#define GSV2K11_VSP_get_DG_EN(port, pval)                              AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x01, 0x80, 0x7, pval)
#define GSV2K11_VSP_set_DG_EN(port, val)                               AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x01, 0x80, 0x7, val)
#define GSV2K11_VSP_get_DG_PATTERN(port, pval)                         AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x01, 0x7C, 2, pval)
#define GSV2K11_VSP_set_DG_PATTERN(port, val)                          AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x01, 0x7C, 2, val)
#define GSV2K11_VSP_get_DG_420_EN(port, pval)                          AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x02, 0x1, 0x0, pval)
#define GSV2K11_VSP_set_DG_420_EN(port, val)                           AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x02, 0x1, 0x0, val)
#define GSV2K11_VSP_set_CP_CSC_MODE(port, val)                         AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x38, 0x3, 0, val)
#define GSV2K11_VSP_get_VIN_H_TOTAL(port, pval)                        AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x52, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_H_TOTAL(port, val)                         AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x52, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_H_ACTIVE(port, pval)                       AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x54, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_H_ACTIVE(port, val)                        AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x54, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_H_FRONT(port, pval)                        AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x56, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_H_FRONT(port, val)                         AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x56, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_H_SYNC(port, pval)                         AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x58, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_H_SYNC(port, val)                          AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x58, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_H_BACK(port, pval)                         AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x5A, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_H_BACK(port, val)                          AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x5A, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_V_TOTAL(port, pval)                        AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x5C, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_V_TOTAL(port, val)                         AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x5C, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_V_ACTIVE(port, pval)                       AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x5E, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_V_ACTIVE(port, val)                        AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x5E, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_V_FRONT(port, pval)                        AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x60, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_V_FRONT(port, val)                         AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x60, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_V_SYNC(port, pval)                         AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x62, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_V_SYNC(port, val)                          AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x62, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_VIN_V_BACK(port, pval)                         AvHalI2cReadField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x64, 0x1F, 0xFF, 0, 2, pval)
#define GSV2K11_VSP_set_VIN_V_BACK(port, val)                          AvHalI2cWriteField32BE(GSV2K11_VSP_MAP_ADDR(port), 0x64, 0x1F, 0xFF, 0, 2, val)
#define GSV2K11_VSP_get_CP_VID_IN(port, pval)                          AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x66, 0xFF, 0, pval)
#define GSV2K11_VSP_set_CP_VID_IN(port, val)                           AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x66, 0xFF, 0, val)
#define GSV2K11_VSP_get_CP_VIN_PARAM_MAN_EN(port, pval)                AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x68, 0x1, 0x0, pval)
#define GSV2K11_VSP_set_CP_VIN_PARAM_MAN_EN(port, val)                 AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x68, 0x1, 0x0, val)
#define GSV2K11_VSP_get_CP_VOUT_CHN_SWAP(port, pval)                   AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x78, 0x70, 4, pval)
#define GSV2K11_VSP_set_CP_VOUT_CHN_SWAP(port, val)                    AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x78, 0x70, 4, val)
#define GSV2K11_VSP_get_CP_VIN_CHN_SWAP(port, pval)                    AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x78, 0x7, 0, pval)
#define GSV2K11_VSP_set_CP_VIN_CHN_SWAP(port, val)                     AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x78, 0x7, 0, val)
#define GSV2K11_VSP_set_SCAL_VOUT_PARAM_SEL(port, val)                 AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x82, 0x6, 1, val)
#define GSV2K11_VSP_get_SCAL_VIN_PARAM_MAN_EN(port, pval)              AvHalI2cReadField8(GSV2K11_VSP_MAP_ADDR(port), 0x82, 0x1, 0x0, pval)
#define GSV2K11_VSP_set_SCAL_VIN_PARAM_MAN_EN(port, val)               AvHalI2cWriteField8(GSV2K11_VSP_MAP_ADDR(port), 0x82, 0x1, 0x0, val)
#endif
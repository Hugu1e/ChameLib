import os

CH_scheme = [
    "CH_CDK_2017",
    "CH_ET_BC_CDK_2017",
    "CH_ET_KOG_CDK_2017",
    "CH_FS_ECC_CCT_2024",
    "CH_KEF_CZK_2004",
    "CH_KEF_DL_CZT_2011",
    "CH_KEF_DLP_LLA_2012",
    "CH_KEF_MH_RSA_F_AM_2004",
    "CH_KEF_MH_RSANN_F_AM_2004",
    "CH_KEF_MH_SDH_DL_AM_2004",
    "CH_KEF_NoMH_AM_2004",
    "CHET_RSA_CDK_2017",
    "CR_CH_DSS_2020",
    "FCR_CH_PreQA_DKS_2020",
    "MCH_CDK_2017"
]

IBCH_scheme = [
    "IB_CH_KEF_CZS_2014",
    "IB_CH_MD_LSX_2022",
    "IB_CH_ZSS_S1_2003",
    "IB_CH_ZSS_S2_2003",
    "ID_B_CollRes_XSL_2021"
]

PBCH_scheme = [
    "DPCH_MXN_2022",
    "MAPCH_ZLW_2021",
    "PCH_DSS_2019",
    "PCHBA_TLL_2020",
    "RPCH_TMM_2022",
    "RPCH_XNM_2021"
]

done = [
]

os.makedirs(f'./data/', exist_ok=True)
os.system("cd build && cmake .. && make -j4")

rep_times = 1

for filename in CH_scheme + IBCH_scheme + PBCH_scheme: 
    if filename[:-5] in done: continue
    open(f'./data/{filename}_Test.txt', 'w')
    os.system(f'cd ./data && ../build/bin/{filename}_benchmark --repeat {rep_times}')

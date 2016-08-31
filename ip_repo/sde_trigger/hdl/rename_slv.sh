# This shell script replaces the "slv_regxxx with the symbolic macro
# name assigned to the register in sde_trigger_v1_2_S000_AXI.v
#
# 20-May-2015 DFN Initial version.

cat sde_trigger_v1_2_S00_AXI.v | 
sed -e "s/slv_reg016/\`COMPATIBILITY_MUON_TRIG_THR0/" | \
sed -e "s/slv_reg017/\`COMPATIBILITY_MUON_TRIG_THR1/" | \
sed -e "s/slv_reg018/\`COMPATIBILITY_MUON_TRIG_THR2/" | \
sed -e "s/slv_reg020/\`COMPATIBILITY_MUON_TRIG_ENAB/" | \
sed -e "s/slv_reg021/\`COMPATIBILITY_AMIGA_TRIG/" | \
sed -e "s/slv_reg022/\`COMPATIBILITY_MOPS_TRIG_MIN0/" | \
sed -e "s/slv_reg023/\`COMPATIBILITY_MOPS_TRIG_MIN1/" | \
sed -e "s/slv_reg024/\`COMPATIBILITY_MOPS_TRIG_MIN2/" | \
sed -e "s/slv_reg025/\`COMPATIBILITY_MOPS_TRIG_MAX0/" | \
sed -e "s/slv_reg026/\`COMPATIBILITY_MOPS_TRIG_MAX1/" | \
sed -e "s/slv_reg027/\`COMPATIBILITY_MOPS_TRIG_MAX2/" | \
sed -e "s/slv_reg028/\`COMPATIBILITY_MOPS_TRIG_ENAB/" | \
sed -e "s/slv_reg029/\`COMPATIBILITY_MOPS_TRIG_INT/" | \
sed -e "s/slv_reg030/\`COMPATIBILITY_MOPS_TRIG_OCC/" | \
sed -e "s/slv_reg031/\`COMPATIBILITY_MOPS_TRIG_OFS/" | \
sed -e "s/slv_reg032/\`COMPATIBILITY_MUON_BUF_TIMER_A/" | \
sed -e "s/slv_reg033/\`COMPATIBILITY_MUON_BUF_TIMER_B/" | \
sed -e "s/slv_reg034/\`COMPATIBILITY_MUON_BUF_TIME_TAG_A/" | \
sed -e "s/slv_reg035/\`COMPATIBILITY_MUON_BUF_TIME_TAG_B/" | \
sed -e "s/slv_reg036/\`COMPATIBILITY_MUON_BUF_TRIG_MASK/" | \
sed -e "s/slv_reg037/\`COMPATIBILITY_MUON_BUF_CONTROL/" | \
sed -e "s/slv_reg038/\`COMPATIBILITY_MUON_BUF_STATUS/" | \
sed -e "s/slv_reg039/\`COMPATIBILITY_MUON_BUF_WORD_COUNT/" | \
sed -e "s/slv_reg040/\`COMPATIBILITY_SHWR_BUF_TRIG_MASK/" | \
sed -e "s/slv_reg041/\`COMPATIBILITY_SHWR_BUF_TRIG_ID/" | \
sed -e "s/slv_reg043/\`COMPATIBILITY_SHWR_BUF_STATUS/" | \
sed -e "s/slv_reg044/\`COMPATIBILITY_TRIG_RATES/" | \
sed -e "s/slv_reg045/\`COMPATIBILITY_DELAYED_RATES/" | \
sed -e "s/slv_reg046/\`COMPATIBILITY_GLOBAL_CONTROL/" | \
sed -e "s/slv_reg047/\`COMPATIBILITY_ID_A/" | \
sed -e "s/slv_reg048/\`COMPATIBILITY_SB_TRIG_THR0/" | \
sed -e "s/slv_reg049/\`COMPATIBILITY_SB_TRIG_THR1/" | \
sed -e "s/slv_reg050/\`COMPATIBILITY_SB_TRIG_THR2/" | \
sed -e "s/slv_reg052/\`COMPATIBILITY_SB_TRIG_ENAB/" | \
sed -e "s/slv_reg053/\`COMPATIBILITY_RANDOM_TRIG_DELAY_A/" | \
sed -e "s/slv_reg054/\`COMPATIBILITY_RANDOM_TRIG_DELAY_B/" | \
sed -e "s/slv_reg055/\`COMPATIBILITY_RANDOM_TRIG_START/" | \
sed -e "s/slv_reg056/\`COMPATIBILITY_TOT_TRIG_THR0/" | \
sed -e "s/slv_reg057/\`COMPATIBILITY_TOT_TRIG_THR1/" | \
sed -e "s/slv_reg058/\`COMPATIBILITY_TOT_TRIG_THR2/" | \
sed -e "s/slv_reg060/\`COMPATIBILITY_TOT_TRIG_ENAB/" | \
sed -e "s/slv_reg062/\`COMPATIBILITY_TOT_TRIG_OCC/" | \
sed -e "s/slv_reg064/\`COMPATIBILITY_TOTD_TRIG_THR0/" | \
sed -e "s/slv_reg065/\`COMPATIBILITY_TOTD_TRIG_THR1/" | \
sed -e "s/slv_reg066/\`COMPATIBILITY_TOTD_TRIG_THR2/" | \
sed -e "s/slv_reg067/\`COMPATIBILITY_TOTD_TRIG_UP0/" | \
sed -e "s/slv_reg068/\`COMPATIBILITY_TOTD_TRIG_UP1/" | \
sed -e "s/slv_reg069/\`COMPATIBILITY_TOTD_TRIG_UP2/" | \
sed -e "s/slv_reg070/\`COMPATIBILITY_TOTD_TRIG_ENAB/" | \
sed -e "s/slv_reg072/\`COMPATIBILITY_TOTD_TRIG_OCC/" | \
sed -e "s/slv_reg073/\`COMPATIBILITY_TOTD_TRIG_FD/" | \
sed -e "s/slv_reg074/\`COMPATIBILITY_TOTD_TRIG_FN/" | \
sed -e "s/slv_reg080/\`COMPATIBILITY_SCALAR_A_THR0/" | \
sed -e "s/slv_reg081/\`COMPATIBILITY_SCALAR_A_THR1/" | \
sed -e "s/slv_reg082/\`COMPATIBILITY_SCALAR_A_THR2/" | \
sed -e "s/slv_reg084/\`COMPATIBILITY_SCALAR_A_ENAB/" | \
sed -e "s/slv_reg087/\`COMPATIBILITY_SCALAR_A_COUNT/" | \
sed -e "s/slv_reg088/\`COMPATIBILITY_SCALAR_B_THR0/" | \
sed -e "s/slv_reg089/\`COMPATIBILITY_SCALAR_B_THR1/" | \
sed -e "s/slv_reg090/\`COMPATIBILITY_SCALAR_B_THR2/" | \
sed -e "s/slv_reg092/\`COMPATIBILITY_SCALAR_B_ENAB/" | \
sed -e "s/slv_reg095/\`COMPATIBILITY_SCALAR_B_COUNT/" | \
sed -e "s/slv_reg096/\`COMPATIBILITY_SCALAR_C_THR0/" | \
sed -e "s/slv_reg097/\`COMPATIBILITY_SCALAR_C_THR1/" | \
sed -e "s/slv_reg098/\`COMPATIBILITY_SCALAR_C_THR2/" | \
sed -e "s/slv_reg100/\`COMPATIBILITY_SCALAR_C_ENAB/" | \
sed -e "s/slv_reg103/\`COMPATIBILITY_SCALAR_C_COUNT/" | \
sed -e "s/slv_reg123/\`COMPATIBILITY_SHADOW_SHWR_BUF_TRIG_MASK/" | \
sed -e "s/slv_reg124/\`COMPATIBILITY_SHADOW_SHWR_BUF_TRIG_ID/" | \
sed -e "s/slv_reg126/\`COMPATIBILITY_SHADOW_SHWR_BUF_STATUS/" | \
sed -e "s/slv_reg127/\`COMPATIBILITY_ID_B/" | \
sed -e "s/slv_reg128/\`SHWR_BUF_TRIG_MASK/" | \
sed -e "s/slv_reg129/\`SHWR_BUF_TRIG_ID/" | \
sed -e "s/slv_reg130/\`SHWR_BUF_CONTROL/" | \
sed -e "s/slv_reg131/\`SHWR_BUF_STATUS/" >tmp.v



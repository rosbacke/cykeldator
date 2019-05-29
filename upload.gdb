target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2.cfg -c "gdb_port pipe; log_output openocd.log"
load
monitor reset
monitor exit

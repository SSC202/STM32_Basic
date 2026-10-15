#include "ethercatuser.h"
#include "LAN8720.h"
#include <stdio.h>
#include <string.h>

char IOmap[48];
int expectedWKC;
boolean inOP;
uint32_t dorun = 0;

int wkc = 0;

PDO_Outputs *outputs;  
PDO_Inputs  *inputs;  

/**
 * @brief EtherCAT Init
 */
void ecat_init(void)
{
    
    printf("EtherCAT init.\r\n");
    
    // 初始化 SOEM
    if (!ec_init("eth")) 
		{      
        printf("EtherCAT init failed\r\n");
        return;
    }
    printf("EtherCAT init done.\r\n");
    
    // 配置从站 
    if (ec_config(FALSE, &IOmap) > 0) {
        printf("%d slaves found and configured.\r\n", ec_slavecount);
        
        if (ec_slavecount >= 1) {
            printf("Found %s at position %d\r\n", ec_slave[1].name, 1);
            printf("Vendor: 0x%08X, Product: 0x%08X\r\n", 
                   ec_slave[1].eep_man, ec_slave[1].eep_id);
        }
				
				ec_statecheck(0, EC_STATE_PRE_OP, EC_TIMEOUTSTATE);
				HAL_Delay(200);
				
        // 配置 DC 同步
        ec_configdc();
				
        // 映射 PDO
				ec_config_map(&IOmap);
				
        // 配置 SYNC0
        ec_dcsync0(1, TRUE, 1000000, 0);
        
        // 等待 SAFE-OP
        ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE);
        
        // 计算 WKC
        expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
        printf("Calculated workcounter %d\r\n", expectedWKC);
        
        // 请求 OP 状态
        printf("Request operational state for all slaves\r\n");
        ec_slave[0].state = EC_STATE_OPERATIONAL;
        
        // 发送一帧空数据
        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);
        ec_writestate(0);
        
        // 等待进入 OP
				int chk, i;
        do {
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
        } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
        
        // 多次写入确保状态 
        for (i = 0; i < 10; i++) {
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            ec_writestate(0);
        }
        
        // 检查是否成功进入 OP 
        if (ec_slave[0].state == EC_STATE_OPERATIONAL) {
            // 设置 PDO 指针 
            outputs = (PDO_Outputs *)ec_slave[1].outputs;
            inputs  = (PDO_Inputs *)ec_slave[1].inputs;
            
            // 初始化 LED 为关闭
            outputs->d1 = 0;
            outputs->d2 = 0;
            
            dorun = 1;
            inOP = TRUE;
            printf("All slaves reached operational state.\r\n");
        } else {
            printf("Failed to reach OP, state: 0x%02X\r\n", ec_slave[0].state);
        }
    } else {
        printf("No slaves found!\r\n");
    }
}

void ecat_loop(void)
{
    if (!dorun) return;
    
    outputs->d1 = (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_12) == GPIO_PIN_RESET) ? 1 : 0;
    outputs->d2 = (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_10) == GPIO_PIN_RESET) ? 1 : 0;
    
    /* PDO 交换 */
    ec_send_processdata();
    wkc = ec_receive_processdata(EC_TIMEOUTRET);
	
		printf("switch: %d,%d\r\n", inputs->s1, inputs->s2);
}


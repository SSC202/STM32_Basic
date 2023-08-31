/**
  @page N25Q512A_STM32F769I-EVAL external loader example
  
  @verbatim
  ********************* COPYRIGHT(c) 2017 STMicroelectronics *******************
  * @file    QSPI/N25Q512A_STM32F769I-EVAL/readme.txt 
  * @author  MCD Tools Team
  * @brief   Description of the external loader of N25Q512A flash memory for STM32F769I-EVAL.
  ******************************************************************************
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This an example of an external loader of N25Q512A flash memory for STM32F769I-EVAL.
This loader can be used with STM32 ST-LINK Utility and STM32CubeProgrammer.

        
@par Hardware and Software environment  

  - This example runs on STM32F7xx devices.
    
  - This example has been tested with STMicroelectronics STM32F769I-EVAL
    boards and can be easily tailored to any other supported device 
    and development board.

@par How to use it ? 

 - Place the folder "N25Q512A_STM32F769I-EVAL" under the following path inside the STM32F7 Cube firmware package    "STM32Cube_FW_F7_Vx.y.z" folder:
   STM32Cube_FW_F7_Vx.y.z\Projects\STM32F769I-EVAL\Examples\QSPI
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - After build, the loader should be displayed in the external loaders window, ready-to-use


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
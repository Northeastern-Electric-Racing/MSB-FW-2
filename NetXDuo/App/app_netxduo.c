/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @brief   NetXDuo applicative file
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u_nx_ethernet.h"
#include "u_threads.h"
#include "u_ethernet.h"
#include "u_board_config.h" // Include the board configuration header for board-specific definitions
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Define board-specific name and ID */
#ifdef MSB_BOARD_1
    #define BOARD_NAME "MSB Board 1"
    #define BOARD_ID 1
#elif defined(MSB_BOARD_2)
    #define BOARD_NAME "MSB Board 2"
    #define BOARD_ID 2
#else
    #error "No MSB board defined. Please define MSB_BOARD_1 or MSB_BOARD_2."
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_NetXDuo_MEM_POOL */
    (void)byte_pool;
  /* USER CODE END App_NetXDuo_MEM_POOL */
  /* USER CODE BEGIN 0 */
    int status;

    // Log the board name and ID
    PRINTLN_INFO("Initializing Ethernet for %s (ID: %d)...", BOARD_NAME, BOARD_ID);

    // Initialize Ethernet
    status = ethernet1_init();
    if (status != U_SUCCESS) {
        PRINTLN_ERROR("Ethernet initialization failed for %s (ID: %d) (Status: %d)", BOARD_NAME, BOARD_ID, status);
        return NX_NOT_SUCCESSFUL;
    }
    PRINTLN_INFO("Ethernet initialized successfully for %s (ID: %d).", BOARD_NAME, BOARD_ID);

    // Initialize threads
    status = threads_init(byte_pool);
    if (status != U_SUCCESS) {
        PRINTLN_ERROR("Thread initialization failed for %s (ID: %d) (Status: %d)", BOARD_NAME, BOARD_ID, status);
        return NX_NOT_SUCCESSFUL;
    }
    PRINTLN_INFO("Threads initialized successfully for %s (ID: %d).", BOARD_NAME, BOARD_ID);
  /* USER CODE END 0 */

  /* USER CODE BEGIN MX_NetXDuo_Init */
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

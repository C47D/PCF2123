/**  PCF2123 SPI Real time clock/calendar
 * Based on datasheet Rev.6 - 15 July 2013
 *
 * @author Carlos Diaz
 * @version 0.1
 *
 * CHANGELOG:
 * A: First library version.
 */

#ifndef PCF2123_H_
#define PCF2123_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdint.h>
#include <stddef.h>

/* Misc */
#ifndef PCF2123_ASSERT
#define PCF2123_ASSERT(x) do { if (!(x)) { PCF2123_on_assertion(); while(1); } } while (0)
#endif

/* Version information */
#define PCF2123_MAJOR_VERSION	0
#define PCF2123_MINOR_VERSION	1

/* Custom types */

/**
 * @enum pcf2123_reg_t
 */
typedef enum {
	/* Control and status registers */
	PCF2123_REG_CONTROL_1 = 0x00,
	PCF2123_REG_CONTROL_2 = 0x01,
	/* Time and date registers */
	PCF2123_REG_SECONDS = 0x02,
	PCF2123_REG_MINUTES = 0x03,
	PCF2123_REG_HOURS = 0x04,
	PCF2123_REG_DAYS = 0x05,
	PCF2123_REG_WEEKDAYS = 0x06,
	PCF2123_REG_MONTHS = 0x07,
	PCF2123_REG_YEARS = 0x08,
	/* Alarm registers */
	PCF2123_REG_MINUTE_ALARM = 0x09,
	PCF2123_REG_HOUR_ALARM = 0x0A,
	PCF2123_REG_DAY_ALARM = 0x0B,
	PCF2123_REG_WEEKDAY_ALARM = 0x0C,
	/* Offset register */
	PCF2123_REG_OFFSET = 0x0D,
	/* Timer registers */
	PCF2123_REG_TIMER_CLKOUT = 0x0E,
	PCF2123_REG_COUNTDOWN_TIMER = 0x0F,
} pcf2123_reg_t;

/* Control 1 register */
#define PCF2123_EXT_TEST_POS				(7)
#define PCF2123_EXT_TEST_MASK				(1 << PCF2123_EXT_TEST_POS)
#define PCF2123_EXT_TEST_NORMAL_MODE		(0 << PCF2123_EXT_TEST_POS)
#define PCF2123_EXT_TEST_EXTERNAL_MODE		(1 << PCF2123_EXT_TEST_POS)

#define PCF2123_STOP_POS					(5)
#define PCF2123_STOP_MASK					(1 << PCF2123_STOP_POS)
#define PCF2123_STOP_RTC_SOURCE_CLK_RUN		(0 << PCF2123_STOP_POS)
#define PCF2123_STOP_RTC_SOURCE_CLK_STOP	(1 << PCF2123_STOP_POS)

#define PCF2123_SR_POS						(4)
#define PCF2123_SR_MASK						(1 << PCF2123_SR_POS)
#define PCF2123_SR_NO_SW_RESET				(0 << PCF2123_SR_POS)
#define PCF2123_SR_INIT_SW_RESET			(1 << PCF2123_SR_POS)

#define PCF2123_12_24_POS					(2)
#define PCF2123_12_24_MASK					(1 << PCF2123_12_24_POS)
#define PCF2123_12_24_24_HR_MODE			(0 << PCF2123_12_24_POS)
#define PCF2123_12_24_12_HR_MODE			(1 << PCF2123_12_24_POS)

#define PCF2123_CIE_POS						(1)
#define PCF2123_CIE_MASK					(1 << PCF2123_CIE_POS)
#define PCF2123_CIE_INT_DISABLE				(0 << PCF2123_CIE_POS)
#define PCF2123_CIE_INT_ENABLE				(1 << PCF2123_CIE_POS)

/* Control 2 register */

#define PCF2123_MI_POS						(7)
#define PCF2123_MI_MASK						(1 << PCF2123_MI_POS)
#define PCF2123_MI_INT_DISABLE				(0 << PCF2123_MI_POS)
#define PCF2123_MI_INT_ENABLE				(1 << PCF2123_MI_POS)

#define PCF2123_SI_POS						(6)
#define PCF2123_SI_MASK						(1 << PCF2123_SI_POS)
#define PCF2123_SI_INT_DISABLE				(0 << PCF2123_SI_POS)
#define PCF2123_SI_INT_ENABLE				(1 << PCF2123_SI_POS)

#define PCF2123_MSF_POS						(5)
#define PCF2123_MSF_MASK					(1 << PCF2123_MSF_POS)
#define PCF2123_MSF_INT_NOT_GENERATED		(0 << PCF2123_MSF_POS)
#define PCF2123_MSF_INT_GENERATED			(1 << PCF2123_MSF_POS)

#define PCF2123_TI_TP_POS						(4)
#define PCF2123_TI_TP_MASK						(1 << PCF2123_TI_TP_POS)
#define PCF2123_TI_TP_INT_FOLLOWS_TIMER_FLAGS	(0 << PCF2123_TI_TP_POS)
#define PCF2123_TI_TP_INT_PIN_GENERATES_PULSE	(1 << PCF2123_TI_TP_POS)

#define PCF2123_AF_POS						(3)
#define PCF2123_AF_MASK						(1 << PCF2123_AF_POS)
#define PCF2123_AF_INT_NOT_GENERATED		(0 << PCF2123_AF_POS)
#define PCF2123_AF_INT_GENERATED			(1 << PCF2123_AF_POS)

#define PCF2123_TF_POS						(2)
#define PCF2123_TF_MASK						(1 << PCF2123_TF_POS)
#define PCF2123_TF_INT_NOT_GENERTED			(0 << PCF2123_TF_POS)
#define PCF2123_TF_INTERRUPT_GENERTED		(1 << PCF2123_TF_POS)

#define PCF2123_AIF_POS						(1)
#define PCF2123_AIF_MASK					(1 << PCF2123_AIF_POS)
#define PCF2123_AIF_INT_DISABLE				(0 << PCF2123_AIF_POS)
#define PCF2123_AIF_INT_ENABLE				(1 << PCF2123_AIF_POS)

#define PCF2123_TIE_POS						(0)
#define PCF2123_TIE_MASK					(1 << PCF2123_TIE_POS)
#define PCF2123_TIE_INT_DISABLE				(0 << PCF2123_TIE_POS)
#define PCF2123_TIE_INT_ENABLE				(1 << PCF2123_TIE_POS)

/* Seconds register */
#define PCF2123_OS_POS						(7)
#define PCF2123_OS_MASK						(1 << PCF2123_OS_POS)
#define PCF2123_OS_INTEGRITY_GUARANTEED		(0 << PCF2123_OS_POS)
#define PCF2123_OS_INTEGRITY_NOT_GUARANTEED	(1 << PCF2123_OS_POS)

#define PCF2123_ALARM_POS					(7)
#define PCF2123_ALARM_ENABLE				(0 << PCF2123_ALARM_POS)
#define PCF2123_ALARM_DISABLE				(1 << PCF2123_ALARM_POS)

/* Public API */

typedef enum {
	PCF2123_WEEKDAY_INVALID		= -1,
	PCF2123_WEEKDAY_SUNDAY		= 0,
	PCF2123_WEEKDAY_MONDAY,
	PCF2123_WEEKDAY_TUESDAY,
	PCF2123_WEEKDAY_WEDNESDAY,
	PCF2123_WEEKDAY_THURSDAY,
	PCF2123_WEEKDAY_FRIDAY,
	PCF2123_WEEKDAY_SATURDAY,
} pcf2123_weekday_t;

typedef enum {
	PCF2123_MONTH_INVALID	= -1,
	PCF2123_MONTH_JANUARY	= 1,
	PCF2123_MONTH_FEBRUARY,
	PCF2123_MONTH_MARCH,
	PCF2123_MONTH_APRIL,
	PCF2123_MONTH_MAY,
	PCF2123_MONTH_JUNE,
	PCF2123_MONTH_JULY,
	PCF2123_MONTH_AUGUST,
	PCF2123_MONTH_SEPTEMBER,
	PCF2123_MONTH_OCTOBER	= 10,
	PCF2123_MONTH_NOVEMBER,
	PCF2123_MONTH_DECEMBER,
} pcf2123_month_t;

typedef enum {
	PCF2123_ETIMEOUT	= -1,
	PCF2123_ENONE		= 0,
} pcf2123_error_t;

typedef enum {
	PCF2123_CE_DISABLE	= 0,
	PCF2123_CE_ENABLE	= 1,
} pcf2123_ce_t;

typedef pcf2123_error_t (*spi_xfer)(uint8_t *write, uint8_t *read, size_t xfer_len, uint32_t timeout_ms);
typedef void (*control_ce)(pcf2123_ce_t ce_state);

typedef struct _pcf2123 pcf2123_t;

struct _pcf2123 {
	spi_xfer	spi_xfer_cb;
	control_ce	control_ce_cb;
};

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
} pcf2123_time_t;

typedef struct {
	uint8_t				day;
	pcf2123_weekday_t	weekday;
	pcf2123_month_t		month;
	uint8_t				year;
} pcf2123_date_t;

#define PCF2123_ALARM_MIN_ENABLE		(1 << 0)
#define PCF2123_ALARM_MIN_DISABLE		(0 << 0)
#define PCF2123_ALARM_HOUR_ENABLE		(1 << 1)
#define PCF2123_ALARM_HOUR_DISABLE		(0 << 1)
#define PCF2123_ALARM_DAY_ENABLE		(1 << 2)
#define PCF2123_ALARM_DAY_DISABLE		(0 << 2)
#define PCF2123_ALARM_WEEKDAY_ENABLE	(1 << 3)
#define PCF2123_ALARM_WEEKDAY_DISABLE	(0 << 3)

typedef struct {
	uint8_t				alarm_enable;
	uint8_t 			min;
	uint8_t 			hour;
	uint8_t 			day;
	pcf2123_weekday_t	weekday;
} pcf2123_alarm_conf_t;

int PCF2123_init(pcf2123_t *pcf, spi_xfer spi_xfer, control_ce control_ce);

int PCF2123_set_rtcc_data(pcf2123_t *pcf, pcf2123_time_t *time, pcf2123_date_t *date);
int PCF2123_get_rtcc_data(pcf2123_t *pcf, pcf2123_time_t *time, pcf2123_date_t *date);

int PCF2123_set_alarm_interrupt(pcf2123_t *pcf, pcf2123_alarm_conf_t *alarm_conf);
int PCF2123_clear_alarm_flag(pcf2123_t *pcf);

int PCF2123_sw_reset(pcf2123_t *pcf);

void pcf2123_enable(pcf2123_t *pcf);
void pcf2123_disable(pcf2123_t *pcf);

void PCF2123_read_register(pcf2123_t *pcf, pcf2123_reg_t reg, uint8_t *data, size_t data_len);
void PCF2123_write_register(pcf2123_t *pcf, pcf2123_reg_t reg, uint8_t *data, size_t data_len);
int	PCF2123_is_af_set(pcf2123_t *pcf);
void PCF2123_clear_af(pcf2123_t *pcf);
int	PCF2123_is_tf_set(pcf2123_t *pcf);
void PCF2123_clear_tf(pcf2123_t *pcf);

uint8_t PCF2123_get_interrupt_flags(pcf2123_t *pcf);
void PCF2123_clear_all_interrupt_flags(pcf2123_t *pcf);

void PCF2123_on_assertion(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PCF2123_H_ */

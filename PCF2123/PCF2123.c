/**  PCF2123 SPI Real time clock/calendar
 * Based on datasheet Rev.6 - 15 July 2013
 *
 * @author Carlos Diaz
 * @version A
 *
 * CHANGELOG:
 * A: First library version.
 */

#include "PCF2123.h"

#define PCF2123_SW_RESET_MAGIC	(0x58)
#define PCF2123_WRITE_DATA		(0x00)
#define PCF2123_READ_DATA		(0x80)
#define PCF2123_SUBADDRESS		(0x10)

/* You can overwrite PCF2123_TIMEOUT_MS if you define it before including PCF2123.h */
#ifndef PCF2123_TIMEOUT_MS
#define PCF2123_TIMEOUT_MS		(500)
#endif

static uint8_t _to_bcd(uint8_t data);
static uint8_t _from_bcd(uint8_t bcd);

int PCF2123_init(pcf2123_t *pcf, spi_xfer spi_xfer, control_ce control_ce)
{
	pcf->control_ce_cb = control_ce;
	pcf->spi_xfer_cb = spi_xfer;

	pcf2123_disable(pcf);

	return PCF2123_ENONE;
}

int PCF2123_set_rtcc_data(pcf2123_t *pcf, pcf2123_time_t *time, pcf2123_date_t *date)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(time);
	PCF2123_ASSERT(date);

	uint8_t data[] = {
		_to_bcd(time->sec), _to_bcd(time->min), _to_bcd(time->hour),
		_to_bcd(date->day), _to_bcd(date->weekday), _to_bcd(date->month), _to_bcd(date->year)
	};

	PCF2123_write_register(pcf, PCF2123_REG_SECONDS,
			data, sizeof data);

	return PCF2123_ENONE;
}

int PCF2123_get_rtcc_data(pcf2123_t *pcf, pcf2123_time_t *time, pcf2123_date_t *date)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(time);
	PCF2123_ASSERT(date);

	uint8_t clear = 0;
	PCF2123_read_register(pcf, PCF2123_REG_SECONDS, &clear, sizeof clear);

	/* Clear OS flag if it is set. */
	if (PCF2123_OS_INTEGRITY_NOT_GUARANTEED & clear) {
		clear |= PCF2123_OS_INTEGRITY_NOT_GUARANTEED;
		PCF2123_write_register(pcf, PCF2123_REG_SECONDS, &clear, sizeof clear);
	}

	/* NOTE: See datasheet 8.4.8 */
	uint8_t data[7] = {0};

	PCF2123_read_register(pcf, PCF2123_REG_SECONDS, data, sizeof data);

	time->sec = _from_bcd(data[0]);
	time->min = _from_bcd(data[1]);
	time->hour = _from_bcd(data[2]);

	date->day = _from_bcd(data[3]);
	date->weekday = _from_bcd(data[4]);
	date->month = _from_bcd(data[5]);
	date->year = _from_bcd(data[6]);

	return PCF2123_ENONE;
}

int PCF2123_set_date(pcf2123_t *pcf, pcf2123_date_t *date)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(date);

	return PCF2123_ENONE;
}

int PCF2123_get_date(pcf2123_t *pcf, pcf2123_date_t *date)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(date);

	uint8_t date_data[4] = {0};

	PCF2123_read_register(pcf, PCF2123_REG_DAYS, date_data, sizeof date_data);

	date->day 		= _from_bcd(date_data[0]);
	date->weekday 	= _from_bcd(date_data[1]);
	date->month 	= _from_bcd(date_data[2]);
	date->year 		= _from_bcd(date_data[3]);

	return PCF2123_ENONE;
}

int PCF2123_set_alarm_interrupt(pcf2123_t *pcf, pcf2123_alarm_conf_t *alarm_conf)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(alarm_conf);

	uint8_t min_alarm 		= alarm_conf->alarm_enable & PCF2123_ALARM_MIN_ENABLE ? _to_bcd(alarm_conf->min) : PCF2123_ALARM_DISABLE;
	uint8_t hour_alarm 		= alarm_conf->alarm_enable & PCF2123_ALARM_HOUR_ENABLE ? _to_bcd(alarm_conf->hour) : PCF2123_ALARM_DISABLE;
	uint8_t day_alarm 		= alarm_conf->alarm_enable & PCF2123_ALARM_DAY_ENABLE ? _to_bcd(alarm_conf->day) : PCF2123_ALARM_DISABLE;
	uint8_t weekday_alarm	= alarm_conf->alarm_enable & PCF2123_ALARM_WEEKDAY_ENABLE ? _to_bcd(alarm_conf->weekday) : PCF2123_ALARM_DISABLE;

	uint8_t cntl_2 = 0;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2, &cntl_2, sizeof cntl_2);

	/* Clear AF bit */
	if (cntl_2 & PCF2123_AF_INT_GENERATED) {
		cntl_2 &= ~(PCF2123_AF_INT_GENERATED);
		PCF2123_write_register(pcf, PCF2123_REG_CONTROL_2, &cntl_2, sizeof cntl_2);
	}

	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2, &cntl_2, sizeof cntl_2);

	/* if AIF is not set */
	if (~(cntl_2 & PCF2123_AIF_INT_ENABLE)) {
		/* Set AIF bit in Control Register 1 */
		cntl_2 |= PCF2123_AIF_INT_ENABLE;
		PCF2123_write_register(pcf, PCF2123_REG_CONTROL_2, &cntl_2, sizeof cntl_2);
	}

	PCF2123_write_register(pcf, PCF2123_REG_MINUTE_ALARM,
			&min_alarm, sizeof min_alarm);
	PCF2123_write_register(pcf, PCF2123_REG_HOUR_ALARM,
			&hour_alarm, sizeof hour_alarm);
	PCF2123_write_register(pcf, PCF2123_REG_DAY_ALARM,
			&day_alarm, sizeof day_alarm);
	PCF2123_write_register(pcf, PCF2123_REG_WEEKDAY_ALARM,
			&weekday_alarm, sizeof weekday_alarm);

	return PCF2123_ENONE;
}

int PCF2123_clear_alarm_flag(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t cntl_2 = 0;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2, &cntl_2, sizeof cntl_2);

	/* If AF is set write 0 to it to clear it */
	if (PCF2123_AF_INT_GENERATED & cntl_2) {
		cntl_2 &= ~(PCF2123_AF_INT_GENERATED);
		PCF2123_write_register(pcf, PCF2123_REG_CONTROL_2, &cntl_2, sizeof cntl_2);
	}

	return PCF2123_ENONE;
}

int PCF2123_sw_reset(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t magic_number = PCF2123_SW_RESET_MAGIC;
	PCF2123_write_register(pcf, PCF2123_REG_CONTROL_1,
			&magic_number, sizeof magic_number);

	return PCF2123_ENONE;
}

void pcf2123_enable(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	pcf->control_ce_cb(PCF2123_CE_ENABLE);
}

void pcf2123_disable(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	pcf->control_ce_cb(PCF2123_CE_DISABLE);
}

void PCF2123_read_register(pcf2123_t *pcf, pcf2123_reg_t reg, uint8_t *data, size_t data_len)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(data);
	PCF2123_ASSERT(0 < data_len);

	uint8_t cmd = PCF2123_READ_DATA | PCF2123_SUBADDRESS | (uint8_t) reg;
	uint8_t write_buf[data_len + 1];
	uint8_t read_buf[data_len + 1];

	write_buf[0] = cmd;

	pcf2123_enable(pcf);
	pcf->spi_xfer_cb(write_buf, read_buf, data_len + 1, PCF2123_TIMEOUT_MS);
	pcf2123_disable(pcf);

	for (size_t idx = 0; idx < data_len; idx++) {
		data[idx] = read_buf[idx + 1];
	}
}

/* The command byte defines the address of the first register to be accessed
	 * and the read/write mode. The address counter will auto increment after every
	 * access and will rollover to zero after the last regoster is accessed. */
void PCF2123_write_register(pcf2123_t *pcf, pcf2123_reg_t reg, uint8_t *data, size_t data_len)
{
	PCF2123_ASSERT(pcf);
	PCF2123_ASSERT(data);
	PCF2123_ASSERT(0 < data_len);

	uint8_t cmd = PCF2123_WRITE_DATA | PCF2123_SUBADDRESS | (uint8_t) reg;
	uint8_t write_buf[data_len + 1];
	uint8_t read_buf[data_len + 1];

	write_buf[0] = cmd;

	for (size_t idx = 1; idx < (data_len + 1); idx++) {
			write_buf[idx] = data[idx - 1];
	}

	pcf2123_enable(pcf);
	pcf->spi_xfer_cb(write_buf, read_buf, data_len + 1, PCF2123_TIMEOUT_MS);
	pcf2123_disable(pcf);
}

int	PCF2123_is_af_set(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t control_2;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);

	return PCF2123_AF_INT_GENERATED & control_2;
}

void PCF2123_clear_af(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t control_2;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);

	control_2 &= ~(PCF2123_AF_INT_GENERATED);

	PCF2123_write_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);
}

int	PCF2123_is_tf_set(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t control_2;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);

	return PCF2123_TF_INTERRUPT_GENERTED & control_2;
}

void PCF2123_clear_tf(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t control_2;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);

	control_2 &= ~(PCF2123_TF_INTERRUPT_GENERTED);

	PCF2123_write_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);
}

uint8_t PCF2123_get_interrupt_flags(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t control_2;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);

	return control_2;
}

void PCF2123_clear_all_interrupt_flags(pcf2123_t *pcf)
{
	PCF2123_ASSERT(pcf);

	uint8_t control_2;
	PCF2123_read_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);

	control_2 |= (0x08 | 0x04 | 0x20);

	PCF2123_write_register(pcf, PCF2123_REG_CONTROL_2,
			&control_2, sizeof control_2);
}

/* Source: http://www.mbeddedc.com/2017/03/decimal-to-binary-coded-decimal-bcd.html */
static uint8_t _to_bcd(uint8_t data)
{
	return ((data / 10) << 4) | (data % 10);
}

/* Source: http://www.mbeddedc.com/2017/03/decimal-to-binary-coded-decimal-bcd.html */
static uint8_t _from_bcd(uint8_t bcd)
{
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

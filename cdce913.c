/*
 * Driver for Texas Instruments
 * Programmable 1-PLL VCXO Clock Synthesizer CDCE913
 *
 * Copyright (C) 2011 Thesys-Intechna
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/i2c.h>

#define DRV_NAME	"cdce913"

/*
struct cdce913_data_str {
	struct i2c_client *client;
	struct mutex lock;
	u16 pdiv[3];
	u32 pll1_0;
	u32 pll1_1;
} cdce913_data;
*/

static int cdce913_read(struct i2c_client *client, u8 reg)
{
	int ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		dev_err(&client->dev, "Read Error\n");

	return ret;
}

static ssize_t cdce913_show_pdiv(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	//unsigned char val;
	//struct spi_device *spi = to_spi_device(dev);
	//spi_read(spi, &val, sizeof(val));
	//return scnprintf(buf, PAGE_SIZE, "pdiv1: 0x%04X; pdiv2: 0x%02X; pdiv3: 0x%02X;\n", cdce913_data.pdiv[0], cdce913_data.pdiv[1], cdce913_data.pdiv[2]);
	return scnprintf(buf, PAGE_SIZE, "NULL\n");
}

static ssize_t cdce913_store_pdiv(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	u8 pdiv_num;
	u16 pdiv_value;
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	// ! (pdiv_num > 3)
	// ! (pdiv_value > 1023)
	if(tmp&0xCC00UL)
		return -EINVAL;
	pdiv_num=(u8)((tmp&0xF000UL)>>12);
	pdiv_value=(u8)(tmp&0x03FFUL);
	if(pdiv_num==0)
		return -EINVAL;
	if((pdiv_num>1)&&(pdiv_value>127))
		return -EINVAL;
	//cdce913_data.pdiv[pdiv_num-1] = pdiv_value;
	return count;
}

static DEVICE_ATTR(pdiv, S_IWUSR|S_IRUSR, cdce913_show_pdiv, cdce913_store_pdiv);
/*
static DEVICE_ATTR(pdiv2, S_IWUSR|S_IRUSR, cdce913_show_pdiv2, cdce913_store_pdiv2);
static DEVICE_ATTR(pdiv3, S_IWUSR|S_IRUSR, cdce913_show_pdiv3, cdce913_store_pdiv3);
static DEVICE_ATTR(y1, S_IWUSR|S_IRUSR, cdce913_show_y1, cdce913_store_y1);
static DEVICE_ATTR(y2y3, S_IWUSR|S_IRUSR, cdce913_show_y2y3, cdce913_store_y2y3);
static DEVICE_ATTR(fs1, S_IWUSR|S_IRUSR, cdce913_show_fs1, cdce913_store_fs1);
static DEVICE_ATTR(pll1_0, S_IWUSR|S_IRUSR, cdce913_show_pll1_0, cdce913_store_pll1_0);
static DEVICE_ATTR(pll1_1, S_IWUSR|S_IRUSR, cdce913_show_pll1_1, cdce913_store_pll1_1);
static DEVICE_ATTR(ssc1, S_IWUSR|S_IRUSR, cdce913_show_ssc1, cdce913_store_ssc1);
*/
static struct attribute *cdce913_attributes[] = {
	&dev_attr_pdiv.attr,
	//&dev_attr_mode.attr,
	NULL
};

static const struct attribute_group cdce913_attr_group = {
	.attrs = cdce913_attributes,
};

static int __devinit cdce913_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	//int ret = 0;
	
	if (!i2c_check_functionality(client->adapter,
					I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "SMBUS Byte Data not Supported\n");
		return -EIO;
	}
	
	cdce913_read(client, 1);
	
	return sysfs_create_group(&client->dev.kobj, &cdce913_attr_group);
}

static int __devexit cdce913_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &cdce913_attr_group);
	return 0;
}

static const struct i2c_device_id cdce913_id[] = {
	{DRV_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, cdce913_id);

static struct i2c_driver cdce913_driver = {
	.driver = {
		   .name = DRV_NAME,
		   },
	.probe = cdce913_probe,
	.remove = __devexit_p(cdce913_remove),
	.id_table = cdce913_id,
};

static int __init cdce913_init(void)
{
	
	return i2c_add_driver(&cdce913_driver);
}

module_init(cdce913_init);

static void __exit cdce913_exit(void)
{
	i2c_del_driver(&cdce913_driver);
}

module_exit(cdce913_exit);

MODULE_AUTHOR("Lapin Roman <lampus.lapin@gmail.com>");
MODULE_DESCRIPTION("TI CDCE913 Clk Driver");
MODULE_LICENSE("GPL");

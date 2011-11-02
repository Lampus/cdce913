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
#include "cdce913.h"

#define DRV_NAME	"cdce913"

union pll_conf {
	struct {
		unsigned vco_range: 2;
		unsigned p: 3;
		unsigned q: 6;
		unsigned r: 9;
		unsigned n: 12;
	};
	u32 data;
	u8 darr[4];
};

struct cdce913_pll {
	
	struct i2c_client *client;
	struct mutex lock;
	u16 pdiv[3];
	u8 y1;
	u8 y2y3;
	u8 fs1;
	u8 ssc1[8];
	union pll_conf pll[2];
};

static int cdce913_read(struct i2c_client *client, u8 reg_addr)
{
	int ret = i2c_smbus_read_byte_data(client, reg_addr);

	if (ret < 0)
		dev_err(&client->dev, "Read Error\n");

	return ret;
}

static int cdce913_write(struct i2c_client *client, u8 reg_addr, u8 value)
{
	int ret = i2c_smbus_write_byte_data(client, reg_addr, value);

	if (ret < 0)
		dev_err(&client->dev, "Write Error\n");

	return ret;
}

static int cdce913_bf_ins(struct i2c_client *client, u8 reg_addr, u8 bf_offset, u8 bf_size, u8 value)
{
	u8 new_value;
	int ret;
	
	ret = (u8)cdce913_read(client, reg_addr);
	if(ret < 0)
		return ret;
	new_value = (u8)ret;
	printk(KERN_DEBUG"Ret: %08X;\n", ret);
	printk(KERN_DEBUG"Addr: %02X; Offs: %02X; SZ: %02X; Val: %02X; NVal: %02X\n", reg_addr, bf_offset, bf_size, value, new_value);
	new_value &= ~(((1 << bf_size) - 1)<<bf_offset);
	printk(KERN_DEBUG"After mask: %02X\n", new_value);
	new_value |= ((value&((1 << bf_size) - 1))<<bf_offset);
	printk(KERN_DEBUG"After insert: %02X\n", new_value);
	ret = cdce913_write(client, reg_addr, new_value);
	if(ret < 0)
		return ret;
	
	return 0;
}

static ssize_t cdce913_show_pdiv(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X,0x%02X,0x%02X\n", dev_data->pdiv[0], dev_data->pdiv[1], dev_data->pdiv[2]);
}

static ssize_t cdce913_store_pdiv(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	u8 pdiv_num;
	u16 pdiv_value;
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	// ! (pdiv_num > 3)
	// ! (pdiv_value > 1023)
	if(tmp&0xCC00UL)
		return -EINVAL;
	pdiv_num=(u8)((tmp&0xF000UL)>>12);
	pdiv_value=(u16)(tmp&0x03FFUL);
	if(pdiv_num==0)
		return -EINVAL;
	if((pdiv_num>1)&&(pdiv_value>127))
		return -EINVAL;
	mutex_lock(&dev_data->lock);
	dev_data->pdiv[pdiv_num-1] = pdiv_value;
	switch(pdiv_num) {
		case 1:
		cdce913_bf_ins(client, CDCE913_RPARAMS(PDIV1_70), (u8)pdiv_value);
		cdce913_bf_ins(client, CDCE913_RPARAMS(PDIV1_98), (u8)(pdiv_value>>8));
		break;
		case 2:
		cdce913_bf_ins(client, CDCE913_RPARAMS(PDIV2), (u8)pdiv_value);
		break;
		case 3:
		cdce913_bf_ins(client, CDCE913_RPARAMS(PDIV3), (u8)pdiv_value);
		break;
		default:
		;
	};
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_y1(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", dev_data->y1);
}

static ssize_t cdce913_store_y1(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	if(tmp&0xFFFFFFF0UL)
		return -EINVAL;
	
	mutex_lock(&dev_data->lock);
	dev_data->y1 = (u8)tmp;
	cdce913_write(client, CDCE913_REG(Y1_X), (u8)tmp);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_y2y3(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", dev_data->y2y3);
}

static ssize_t cdce913_store_y2y3(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	if(tmp&0xFFFFFFF0UL)
		return -EINVAL;
	
	mutex_lock(&dev_data->lock);
	dev_data->y2y3 = (u8)tmp;
	cdce913_write(client, CDCE913_REG(Y2Y3_X), (u8)tmp);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_fs1(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", dev_data->fs1);
}

static ssize_t cdce913_store_fs1(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	if(tmp&0xFFFFFFF0UL)
		return -EINVAL;
	
	mutex_lock(&dev_data->lock);
	dev_data->fs1 = (u8)tmp;
	cdce913_write(client, CDCE913_REG(FS1_X), (u8)tmp);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_pll1_0(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%08X\n", dev_data->pll[0].data);
}

static ssize_t cdce913_store_pll1_0(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	u8 i;
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
		
	mutex_lock(&dev_data->lock);
	dev_data->pll[0].data = tmp;
	for(i = 0; i < 4; i++)
		cdce913_write(client, CDCE913_REG(PLL1_0N_11_4) + i, dev_data->pll[0].darr[3-i]);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_pll1_1(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);

	return scnprintf(buf, PAGE_SIZE, "0x%08X\n", dev_data->pll[1].data);
}

static ssize_t cdce913_store_pll1_1(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	u8 i;
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
		
	mutex_lock(&dev_data->lock);
	dev_data->pll[1].data = tmp;
	for(i = 0; i < 4; i++)
		cdce913_write(client, CDCE913_REG(PLL1_1N_11_4) + i, dev_data->pll[1].darr[3-i]);
	mutex_unlock(&dev_data->lock);
	return count;
}

static ssize_t cdce913_show_ssc1(struct device *dev,
				struct device_attribute *attr,
                char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client);
	u8 i;
	ssize_t ret_sz;
	
	scnprintf(buf, PAGE_SIZE, "0x%02X", dev_data->ssc1[0]);
	for(i = 1; i < 8; i++) {
		scnprintf(buf, PAGE_SIZE, "%s,0x%02X", buf, dev_data->ssc1[i]);
	}
	ret_sz = scnprintf(buf, PAGE_SIZE, "%s\n", buf);
	return ret_sz;
}

static ssize_t cdce913_store_ssc1(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned long tmp;
	struct i2c_client *client = to_i2c_client(dev);
	struct cdce913_pll *dev_data = i2c_get_clientdata(client); 
	u8 ssc_num;
	u8 ssc_value;
	if (strict_strtoul(buf, 16, &tmp) < 0)
		return -EINVAL;
	// ! (pdiv_num > 3)
	// ! (pdiv_value > 1023)
	if(tmp&0xCC00UL)
		return -EINVAL;
	ssc_num=(u8)((tmp&0x000000F0UL)>>4);
	ssc_value=(u8)(tmp&0x0000000F);
	if((ssc_num > 7) || (ssc_value > 7))
		return -EINVAL;
	mutex_lock(&dev_data->lock);
	dev_data->ssc1[ssc_num] = ssc_value;
	// Yep, this switch is ugly, but it's simple
	switch(ssc_num) {
		case 0:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_0_20), ssc_value);
			break;
		case 1:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_1_20), ssc_value);
			break;
		case 2:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_2_2), (ssc_value&0x04)>>2);
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_2_10), ssc_value&0x03);
			break;
		case 3:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_3_20), ssc_value);
			break;
		case 4:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_4_20), ssc_value);
			break;
		case 5:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_5_21), (ssc_value&0x06)>>1);
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_5_0), ssc_value&0x01);
			break;
		case 6:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_6_20), ssc_value);
			break;
		case 7:
			cdce913_bf_ins(client, CDCE913_RPARAMS(SSC1_7_20), ssc_value);
			break;
		default:
			break;
	}
	mutex_unlock(&dev_data->lock);
	return count;
}


static DEVICE_ATTR(pdiv, S_IWUSR|S_IRUSR, cdce913_show_pdiv, cdce913_store_pdiv);
static DEVICE_ATTR(y1, S_IWUSR|S_IRUSR, cdce913_show_y1, cdce913_store_y1);
static DEVICE_ATTR(y2y3, S_IWUSR|S_IRUSR, cdce913_show_y2y3, cdce913_store_y2y3);
static DEVICE_ATTR(fs1, S_IWUSR|S_IRUSR, cdce913_show_fs1, cdce913_store_fs1);
static DEVICE_ATTR(pll1_0, S_IWUSR|S_IRUSR, cdce913_show_pll1_0, cdce913_store_pll1_0);
static DEVICE_ATTR(pll1_1, S_IWUSR|S_IRUSR, cdce913_show_pll1_1, cdce913_store_pll1_1);
static DEVICE_ATTR(ssc1, S_IWUSR|S_IRUSR, cdce913_show_ssc1, cdce913_store_ssc1);

static struct attribute *cdce913_attributes[] = {
	&dev_attr_pdiv.attr,
	&dev_attr_y1.attr,
	&dev_attr_y2y3.attr,
	&dev_attr_fs1.attr,
	&dev_attr_pll1_0.attr,
	&dev_attr_pll1_1.attr,
	&dev_attr_ssc1.attr,
	NULL
};

static const struct attribute_group cdce913_attr_group = {
	.attrs = cdce913_attributes,
};

static int __devinit cdce913_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	int ret = 0;
	struct cdce913_pll *dev;
	
	if (!i2c_check_functionality(client->adapter,
					I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "SMBUS Byte Data not Supported\n");
		return -EIO;
	}
	
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		dev_err(&client->dev, "failed to alloc memory\n");
		return -ENOMEM;
	}
	mutex_init(&dev->lock);
	
	i2c_set_clientdata(client, dev);
	
	return sysfs_create_group(&client->dev.kobj, &cdce913_attr_group);
	
err:
	kfree(dev);
	return ret;
}

static int __devexit cdce913_remove(struct i2c_client *client)
{
	struct cdce913_pll *dev = i2c_get_clientdata(client);
	kfree(dev);
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

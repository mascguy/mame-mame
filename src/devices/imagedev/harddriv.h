// license:BSD-3-Clause
// copyright-holders:Nathan Woods, Raphael Nabet, Miodrag Milanovic
/*********************************************************************

    harddriv.h

    Interface to the CHD code

*********************************************************************/

#ifndef MAME_DEVICES_IMAGEDEV_HARDDRIV_H
#define MAME_DEVICES_IMAGEDEV_HARDDRIV_H

#include "softlist_dev.h"

#include "chd.h"
#include "harddisk.h"

#include <memory>
#include <string>
#include <system_error>
#include <utility>


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

// ======================> harddisk_image_base_device

class harddisk_image_base_device : public device_t, public device_image_interface
{
protected:
	// construction/destruction
	harddisk_image_base_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device_image_interface implementation
	virtual bool is_readable()  const noexcept override { return true; }
	virtual bool is_writeable() const noexcept override { return true; }
	virtual bool is_creatable() const noexcept override { return false; }
	virtual bool is_reset_on_load() const noexcept override { return false; }
	virtual const char *image_type_name() const noexcept override { return "harddisk"; }
	virtual const char *image_brief_type_name() const noexcept override { return "hard"; }
};

// ======================> harddisk_image_device

class harddisk_image_device : public harddisk_image_base_device
{
public:
	typedef device_delegate<std::error_condition (device_image_interface &)> load_delegate;
	typedef device_delegate<void (device_image_interface &)> unload_delegate;

	// construction/destruction
	harddisk_image_device(const machine_config &mconfig, const char *tag, device_t *owner, const char *intf)
		: harddisk_image_device(mconfig, tag, owner, (uint32_t)0)
	{
		set_interface(intf);
	}
	harddisk_image_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);
	virtual ~harddisk_image_device();

	template <typename... T> void set_device_load(T &&... args) { m_device_image_load.set(std::forward<T>(args)...); }
	template <typename... T> void set_device_unload(T &&... args) { m_device_image_unload.set(std::forward<T>(args)...); }
	void set_interface(const char *interface) { m_interface = interface; }

	// device_image_interface implementation
	virtual std::pair<std::error_condition, std::string> call_load() override;
	virtual std::pair<std::error_condition, std::string> call_create(int create_format, util::option_resolution *create_args) override;
	virtual void call_unload() override;

	virtual bool image_is_chd_type() const noexcept override { return true; }
	virtual const char *image_interface() const noexcept override { return m_interface; }
	virtual const char *file_extensions() const noexcept override { return "chd,hd,hdv,2mg,hdi"; }
	virtual const util::option_guide &create_option_guide() const override;

	// specific implementation
	hard_disk_file *get_hard_disk_file() { return m_hard_disk_handle.get(); }

protected:
	harddisk_image_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device_t implementation
	virtual void device_config_complete() override;
	virtual void device_start() override;
	virtual void device_stop() override;

	// device_image_interface implementation
	virtual const software_list_loader &get_software_list_loader() const override { return rom_software_list_loader::instance(); }

	std::error_condition internal_load_hd();

	chd_file        *m_chd;
	chd_file        m_origchd;              // handle to the original CHD
	chd_file        m_diffchd;              // handle to the diff CHD
	std::unique_ptr<hard_disk_file> m_hard_disk_handle;

	load_delegate   m_device_image_load;
	unload_delegate m_device_image_unload;
	const char *    m_interface;
};

// device type definition
DECLARE_DEVICE_TYPE(HARDDISK, harddisk_image_device)

#endif // MAME_DEVICES_IMAGEDEV_HARDDRIV_H

#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xca05c877, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7d2ad15, __VMLINUX_SYMBOL_STR(pv_cpu_ops) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{        0, __VMLINUX_SYMBOL_STR(sys_close) },
	{ 0x998f5092, __VMLINUX_SYMBOL_STR(commit_creds) },
	{ 0xce9693d0, __VMLINUX_SYMBOL_STR(make_kuid) },
	{ 0x1d8912d1, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x2ce5b3aa, __VMLINUX_SYMBOL_STR(prepare_creds) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "EA7A42DABF9C9ECD276A778");

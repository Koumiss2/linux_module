include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-vping
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_LICENSE:=Apache-2.0
PKG_MAINTAINER:=Anton

LUCI_TITLE:=LuCI support for virtual ping procfs control
LUCI_DESCRIPTION:=LuCI page for configuring the virtual ping kernel module through /proc/proc_ip
LUCI_DEPENDS:=+luci-base +luci-compat +luci-mod-admin-full
LUCI_PKGARCH:=all

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature

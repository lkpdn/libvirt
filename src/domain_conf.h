/*
 * domain_conf.h: domain XML processing
 *
 * Copyright (C) 2006-2008 Red Hat, Inc.
 * Copyright (C) 2006-2008 Daniel P. Berrange
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * Author: Daniel P. Berrange <berrange@redhat.com>
 */

#ifndef __DOMAIN_CONF_H
#define __DOMAIN_CONF_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "internal.h"
#include "capabilities.h"
#include "util.h"
#include "threads.h"

/* Different types of hypervisor */
/* NB: Keep in sync with virDomainVirtTypeToString impl */
enum virDomainVirtType {
    VIR_DOMAIN_VIRT_QEMU,
    VIR_DOMAIN_VIRT_KQEMU,
    VIR_DOMAIN_VIRT_KVM,
    VIR_DOMAIN_VIRT_XEN,
    VIR_DOMAIN_VIRT_LXC,
    VIR_DOMAIN_VIRT_UML,
    VIR_DOMAIN_VIRT_OPENVZ,
    VIR_DOMAIN_VIRT_VSERVER,
    VIR_DOMAIN_VIRT_LDOM,
    VIR_DOMAIN_VIRT_TEST,
    VIR_DOMAIN_VIRT_VMWARE,
    VIR_DOMAIN_VIRT_HYPERV,

    VIR_DOMAIN_VIRT_LAST,
};

/* Two types of disk backends */
enum virDomainDiskType {
    VIR_DOMAIN_DISK_TYPE_BLOCK,
    VIR_DOMAIN_DISK_TYPE_FILE,

    VIR_DOMAIN_DISK_TYPE_LAST
};

/* Three types of disk frontend */
enum virDomainDiskDevice {
    VIR_DOMAIN_DISK_DEVICE_DISK,
    VIR_DOMAIN_DISK_DEVICE_CDROM,
    VIR_DOMAIN_DISK_DEVICE_FLOPPY,

    VIR_DOMAIN_DISK_DEVICE_LAST
};

enum virDomainDiskBus {
    VIR_DOMAIN_DISK_BUS_IDE,
    VIR_DOMAIN_DISK_BUS_FDC,
    VIR_DOMAIN_DISK_BUS_SCSI,
    VIR_DOMAIN_DISK_BUS_VIRTIO,
    VIR_DOMAIN_DISK_BUS_XEN,
    VIR_DOMAIN_DISK_BUS_USB,
    VIR_DOMAIN_DISK_BUS_UML,

    VIR_DOMAIN_DISK_BUS_LAST
};

enum  virDomainDiskCache {
    VIR_DOMAIN_DISK_CACHE_DEFAULT,
    VIR_DOMAIN_DISK_CACHE_DISABLE,
    VIR_DOMAIN_DISK_CACHE_WRITETHRU,
    VIR_DOMAIN_DISK_CACHE_WRITEBACK,

    VIR_DOMAIN_DISK_CACHE_LAST
};

/* Stores the virtual disk configuration */
typedef struct _virDomainDiskDef virDomainDiskDef;
typedef virDomainDiskDef *virDomainDiskDefPtr;
struct _virDomainDiskDef {
    int type;
    int device;
    int bus;
    char *src;
    char *dst;
    char *driverName;
    char *driverType;
    int cachemode;
    unsigned int readonly : 1;
    unsigned int shared : 1;
    int slotnum; /* pci slot number for unattach */
};


/* Two types of disk backends */
enum virDomainFSType {
    VIR_DOMAIN_FS_TYPE_MOUNT,   /* Better named 'bind' */
    VIR_DOMAIN_FS_TYPE_BLOCK,
    VIR_DOMAIN_FS_TYPE_FILE,
    VIR_DOMAIN_FS_TYPE_TEMPLATE,

    VIR_DOMAIN_FS_TYPE_LAST
};

typedef struct _virDomainFSDef virDomainFSDef;
typedef virDomainFSDef *virDomainFSDefPtr;
struct _virDomainFSDef {
    int type;
    char *src;
    char *dst;
    unsigned int readonly : 1;
};


/* 5 different types of networking config */
enum virDomainNetType {
    VIR_DOMAIN_NET_TYPE_USER,
    VIR_DOMAIN_NET_TYPE_ETHERNET,
    VIR_DOMAIN_NET_TYPE_SERVER,
    VIR_DOMAIN_NET_TYPE_CLIENT,
    VIR_DOMAIN_NET_TYPE_MCAST,
    VIR_DOMAIN_NET_TYPE_NETWORK,
    VIR_DOMAIN_NET_TYPE_BRIDGE,

    VIR_DOMAIN_NET_TYPE_LAST,
};


/* Stores the virtual network interface configuration */
typedef struct _virDomainNetDef virDomainNetDef;
typedef virDomainNetDef *virDomainNetDefPtr;
struct _virDomainNetDef {
    int type;
    unsigned char mac[VIR_MAC_BUFLEN];
    char *model;
    union {
        struct {
            char *dev;
            char *script;
            char *ipaddr;
        } ethernet;
        struct {
            char *address;
            int port;
        } socket; /* any of NET_CLIENT or NET_SERVER or NET_MCAST */
        struct {
            char *name;
        } network;
        struct {
            char *brname;
            char *script;
            char *ipaddr;
        } bridge;
    } data;
    char *ifname;
};

enum virDomainChrSrcType {
    VIR_DOMAIN_CHR_TYPE_NULL,
    VIR_DOMAIN_CHR_TYPE_VC,
    VIR_DOMAIN_CHR_TYPE_PTY,
    VIR_DOMAIN_CHR_TYPE_DEV,
    VIR_DOMAIN_CHR_TYPE_FILE,
    VIR_DOMAIN_CHR_TYPE_PIPE,
    VIR_DOMAIN_CHR_TYPE_STDIO,
    VIR_DOMAIN_CHR_TYPE_UDP,
    VIR_DOMAIN_CHR_TYPE_TCP,
    VIR_DOMAIN_CHR_TYPE_UNIX,

    VIR_DOMAIN_CHR_TYPE_LAST,
};

enum virDomainChrTcpProtocol {
    VIR_DOMAIN_CHR_TCP_PROTOCOL_RAW,
    VIR_DOMAIN_CHR_TCP_PROTOCOL_TELNET,

    VIR_DOMAIN_CHR_TCP_PROTOCOL_LAST,
};

typedef struct _virDomainChrDef virDomainChrDef;
typedef virDomainChrDef *virDomainChrDefPtr;
struct _virDomainChrDef {
    int dstPort;

    int type;
    union {
        struct {
            char *path;
        } file; /* pty, file, pipe, or device */
        struct {
            char *host;
            char *service;
            int listen;
            int protocol;
        } tcp;
        struct {
            char *bindHost;
            char *bindService;
            char *connectHost;
            char *connectService;
        } udp;
        struct {
            char *path;
            int listen;
        } nix;
    } data;
};

enum virDomainInputType {
    VIR_DOMAIN_INPUT_TYPE_MOUSE,
    VIR_DOMAIN_INPUT_TYPE_TABLET,

    VIR_DOMAIN_INPUT_TYPE_LAST,
};

enum virDomainInputBus {
    VIR_DOMAIN_INPUT_BUS_PS2,
    VIR_DOMAIN_INPUT_BUS_USB,
    VIR_DOMAIN_INPUT_BUS_XEN,

    VIR_DOMAIN_INPUT_BUS_LAST
};

typedef struct _virDomainInputDef virDomainInputDef;
typedef virDomainInputDef *virDomainInputDefPtr;
struct _virDomainInputDef {
    int type;
    int bus;
};

enum virDomainSoundModel {
    VIR_DOMAIN_SOUND_MODEL_SB16,
    VIR_DOMAIN_SOUND_MODEL_ES1370,
    VIR_DOMAIN_SOUND_MODEL_PCSPK,
    VIR_DOMAIN_SOUND_MODEL_ES97,

    VIR_DOMAIN_SOUND_MODEL_LAST
};

typedef struct _virDomainSoundDef virDomainSoundDef;
typedef virDomainSoundDef *virDomainSoundDefPtr;
struct _virDomainSoundDef {
    int model;
};

/* 3 possible graphics console modes */
enum virDomainGraphicsType {
    VIR_DOMAIN_GRAPHICS_TYPE_SDL,
    VIR_DOMAIN_GRAPHICS_TYPE_VNC,

    VIR_DOMAIN_GRAPHICS_TYPE_LAST,
};

typedef struct _virDomainGraphicsDef virDomainGraphicsDef;
typedef virDomainGraphicsDef *virDomainGraphicsDefPtr;
struct _virDomainGraphicsDef {
    int type;
    union {
        struct {
            int port;
            int autoport : 1;
            char *listenAddr;
            char *keymap;
            char *passwd;
        } vnc;
        struct {
            char *display;
            char *xauth;
            int fullscreen;
        } sdl;
    } data;
};

enum virDomainHostdevMode {
    VIR_DOMAIN_HOSTDEV_MODE_SUBSYS,
    VIR_DOMAIN_HOSTDEV_MODE_CAPABILITIES,

    VIR_DOMAIN_HOSTDEV_MODE_LAST,
};

enum virDomainHostdevSubsysType {
    VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_USB,
    VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_PCI,

    VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_LAST
};

typedef struct _virDomainHostdevDef virDomainHostdevDef;
typedef virDomainHostdevDef *virDomainHostdevDefPtr;
struct _virDomainHostdevDef {
    int mode; /* enum virDomainHostdevMode */
    unsigned int managed : 1;
    union {
        struct {
            int type; /* enum virDomainHostdevBusType */
            union {
                struct {
                    unsigned bus;
                    unsigned device;

                    unsigned vendor;
                    unsigned product;
                } usb;
                struct {
                     unsigned domain;
                     unsigned bus;
                     unsigned slot;
                     unsigned function;
                } pci;
            } u;
        } subsys;
        struct {
            /* TBD: struct capabilities see:
             * https://www.redhat.com/archives/libvir-list/2008-July/msg00429.html
             */
            int dummy;
        } caps;
    } source;
    char* target;
};

/* Flags for the 'type' field in next struct */
enum virDomainDeviceType {
    VIR_DOMAIN_DEVICE_DISK,
    VIR_DOMAIN_DEVICE_FS,
    VIR_DOMAIN_DEVICE_NET,
    VIR_DOMAIN_DEVICE_INPUT,
    VIR_DOMAIN_DEVICE_SOUND,
    VIR_DOMAIN_DEVICE_HOSTDEV,

    VIR_DOMAIN_DEVICE_LAST,
};

typedef struct _virDomainDeviceDef virDomainDeviceDef;
typedef virDomainDeviceDef *virDomainDeviceDefPtr;
struct _virDomainDeviceDef {
    int type;
    union {
        virDomainDiskDefPtr disk;
        virDomainFSDefPtr fs;
        virDomainNetDefPtr net;
        virDomainInputDefPtr input;
        virDomainSoundDefPtr sound;
        virDomainHostdevDefPtr hostdev;
    } data;
};


#define VIR_DOMAIN_MAX_BOOT_DEVS 4

/* 3 possible boot devices */
enum virDomainBootOrder {
    VIR_DOMAIN_BOOT_FLOPPY,
    VIR_DOMAIN_BOOT_CDROM,
    VIR_DOMAIN_BOOT_DISK,
    VIR_DOMAIN_BOOT_NET,

    VIR_DOMAIN_BOOT_LAST,
};

enum virDomainFeature {
    VIR_DOMAIN_FEATURE_ACPI,
    VIR_DOMAIN_FEATURE_APIC,
    VIR_DOMAIN_FEATURE_PAE,

    VIR_DOMAIN_FEATURE_LAST
};

enum virDomainLifecycleAction {
    VIR_DOMAIN_LIFECYCLE_DESTROY,
    VIR_DOMAIN_LIFECYCLE_RESTART,
    VIR_DOMAIN_LIFECYCLE_RESTART_RENAME,
    VIR_DOMAIN_LIFECYCLE_PRESERVE,

    VIR_DOMAIN_LIFECYCLE_LAST
};

/* Operating system configuration data & machine / arch */
typedef struct _virDomainOSDef virDomainOSDef;
typedef virDomainOSDef *virDomainOSDefPtr;
struct _virDomainOSDef {
    char *type;
    char *arch;
    char *machine;
    int nBootDevs;
    int bootDevs[VIR_DOMAIN_BOOT_LAST];
    char *init;
    char *kernel;
    char *initrd;
    char *cmdline;
    char *root;
    char *loader;
    char *bootloader;
    char *bootloaderArgs;
};

/* Security configuration for domain */
typedef struct _virSecurityLabelDef virSecurityLabelDef;
typedef virSecurityLabelDef *virSecurityLabelDefPtr;
struct _virSecurityLabelDef {
    char *model;        /* name of security model */
    char *label;        /* security label string */
    char *imagelabel;   /* security image label string */
};

#define VIR_DOMAIN_CPUMASK_LEN 1024

/* Guest VM main configuration */
typedef struct _virDomainDef virDomainDef;
typedef virDomainDef *virDomainDefPtr;
struct _virDomainDef {
    int virtType;
    int id;
    unsigned char uuid[VIR_UUID_BUFLEN];
    char *name;

    unsigned long memory;
    unsigned long maxmem;
    unsigned long vcpus;
    int cpumasklen;
    char *cpumask;

    /* These 3 are based on virDomainLifeCycleAction enum flags */
    int onReboot;
    int onPoweroff;
    int onCrash;

    virDomainOSDef os;
    char *emulator;
    int features;

    int localtime;

    /* Only 1 */
    virDomainGraphicsDefPtr graphics;

    int ndisks;
    virDomainDiskDefPtr *disks;

    int nfss;
    virDomainFSDefPtr *fss;

    int nnets;
    virDomainNetDefPtr *nets;

    int ninputs;
    virDomainInputDefPtr *inputs;

    int nsounds;
    virDomainSoundDefPtr *sounds;

    int nhostdevs;
    virDomainHostdevDefPtr *hostdevs;

    int nserials;
    virDomainChrDefPtr *serials;

    int nparallels;
    virDomainChrDefPtr *parallels;

    /* Only 1 */
    virDomainChrDefPtr console;
    virSecurityLabelDef seclabel;
};

/* Guest VM runtime state */
typedef struct _virDomainObj virDomainObj;
typedef virDomainObj *virDomainObjPtr;
struct _virDomainObj {
    virMutex lock;

    int monitor;
    char *monitorpath;
    int monitorWatch;
    int logfile;
    int pid;
    int state;

    int nvcpupids;
    int *vcpupids;

    unsigned int autostart : 1;
    unsigned int persistent : 1;

    virDomainDefPtr def; /* The current definition */
    virDomainDefPtr newDef; /* New definition to activate at shutdown */
};

typedef struct _virDomainObjList virDomainObjList;
typedef virDomainObjList *virDomainObjListPtr;
struct _virDomainObjList {
    unsigned int count;
    virDomainObjPtr *objs;
};

static inline int
virDomainIsActive(virDomainObjPtr dom)
{
    return dom->def->id != -1;
}


virDomainObjPtr virDomainFindByID(const virDomainObjListPtr doms,
                                  int id);
virDomainObjPtr virDomainFindByUUID(const virDomainObjListPtr doms,
                                    const unsigned char *uuid);
virDomainObjPtr virDomainFindByName(const virDomainObjListPtr doms,
                                    const char *name);


void virDomainGraphicsDefFree(virDomainGraphicsDefPtr def);
void virDomainInputDefFree(virDomainInputDefPtr def);
void virDomainDiskDefFree(virDomainDiskDefPtr def);
void virDomainFSDefFree(virDomainFSDefPtr def);
void virDomainNetDefFree(virDomainNetDefPtr def);
void virDomainChrDefFree(virDomainChrDefPtr def);
void virDomainSoundDefFree(virDomainSoundDefPtr def);
void virDomainHostdevDefFree(virDomainHostdevDefPtr def);
void virDomainDeviceDefFree(virDomainDeviceDefPtr def);
void virDomainDefFree(virDomainDefPtr vm);
void virDomainObjFree(virDomainObjPtr vm);
void virDomainObjListFree(virDomainObjListPtr vms);

virDomainObjPtr virDomainAssignDef(virConnectPtr conn,
                                   virDomainObjListPtr doms,
                                   const virDomainDefPtr def);
void virDomainRemoveInactive(virDomainObjListPtr doms,
                             virDomainObjPtr dom);

#ifndef PROXY
virDomainDeviceDefPtr virDomainDeviceDefParse(virConnectPtr conn,
                                              virCapsPtr caps,
                                              const virDomainDefPtr def,
                                              const char *xmlStr,
                                              int flags);
virDomainDefPtr virDomainDefParseString(virConnectPtr conn,
                                        virCapsPtr caps,
                                        const char *xmlStr,
                                        int flags);
virDomainDefPtr virDomainDefParseFile(virConnectPtr conn,
                                      virCapsPtr caps,
                                      const char *filename,
                                      int flags);
virDomainDefPtr virDomainDefParseNode(virConnectPtr conn,
                                      virCapsPtr caps,
                                      xmlDocPtr doc,
                                      xmlNodePtr root,
                                      int flags);
#endif
char *virDomainDefFormat(virConnectPtr conn,
                         virDomainDefPtr def,
                         int flags);

int virDomainCpuSetParse(virConnectPtr conn,
                         const char **str,
                         char sep,
                         char *cpuset,
                         int maxcpu);
char *virDomainCpuSetFormat(virConnectPtr conn,
                            char *cpuset,
                            int maxcpu);

int virDomainDiskQSort(const void *a, const void *b);
int virDomainDiskCompare(virDomainDiskDefPtr a,
                         virDomainDiskDefPtr b);

int virDomainSaveXML(virConnectPtr conn,
                     const char *configDir,
                     virDomainDefPtr def,
                     const char *xml);

int virDomainSaveConfig(virConnectPtr conn,
                        const char *configDir,
                        virDomainDefPtr def);

typedef void (*virDomainLoadConfigNotify)(virDomainObjPtr dom,
                                          int newDomain,
                                          void *opaque);

virDomainObjPtr virDomainLoadConfig(virConnectPtr conn,
                                    virCapsPtr caps,
                                    virDomainObjListPtr doms,
                                    const char *configDir,
                                    const char *autostartDir,
                                    const char *name,
                                    virDomainLoadConfigNotify notify,
                                    void *opaque);

int virDomainLoadAllConfigs(virConnectPtr conn,
                            virCapsPtr caps,
                            virDomainObjListPtr doms,
                            const char *configDir,
                            const char *autostartDir,
                            virDomainLoadConfigNotify notify,
                            void *opaque);

int virDomainDeleteConfig(virConnectPtr conn,
                          const char *configDir,
                          const char *autostartDir,
                          virDomainObjPtr dom);

char *virDomainConfigFile(virConnectPtr conn,
                          const char *dir,
                          const char *name);

int virDiskNameToBusDeviceIndex(virDomainDiskDefPtr disk,
                                int *busIdx,
                                int *devIdx);

const char *virDomainDefDefaultEmulator(virConnectPtr conn,
                                        virDomainDefPtr def,
                                        virCapsPtr caps);

void virDomainObjLock(virDomainObjPtr obj);
void virDomainObjUnlock(virDomainObjPtr obj);

VIR_ENUM_DECL(virDomainVirt)
VIR_ENUM_DECL(virDomainBoot)
VIR_ENUM_DECL(virDomainFeature)
VIR_ENUM_DECL(virDomainLifecycle)
VIR_ENUM_DECL(virDomainDevice)
VIR_ENUM_DECL(virDomainDisk)
VIR_ENUM_DECL(virDomainDiskDevice)
VIR_ENUM_DECL(virDomainDiskBus)
VIR_ENUM_DECL(virDomainDiskCache)
VIR_ENUM_DECL(virDomainFS)
VIR_ENUM_DECL(virDomainNet)
VIR_ENUM_DECL(virDomainChr)
VIR_ENUM_DECL(virDomainSoundModel)
VIR_ENUM_DECL(virDomainHostdevMode)
VIR_ENUM_DECL(virDomainHostdevSubsys)
VIR_ENUM_DECL(virDomainInput)
VIR_ENUM_DECL(virDomainInputBus)
VIR_ENUM_DECL(virDomainGraphics)
/* from libvirt.h */
VIR_ENUM_DECL(virDomainState)

#endif /* __DOMAIN_CONF_H */

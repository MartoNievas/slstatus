//* See LICENSE file for copyright and license details. */

/* Define config*/
/* interval between updates (in ms) */
const unsigned int interval = 300;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "";

/* maximum output string length */
#define MAXLEN 2048

/*
 * function            description                     argument (example)
 *
 * battery_perc        battery percentage              battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_remaining   battery remaining HH:MM         battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_state       battery charging state          battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * cat                 read arbitrary file             path
 * cpu_freq            cpu frequency in MHz            NULL
 * cpu_perc            cpu usage in percent            NULL
 * datetime            date and time                   format string (%F %T)
 * disk_free           free disk space in GB           mountpoint path (/)
 * disk_perc           disk usage in percent           mountpoint path (/)
 * disk_total          total disk space in GB          mountpoint path (/)
 * disk_used           used disk space in GB           mountpoint path (/)
 * entropy             available entropy               NULL
 * gid                 GID of current user             NULL
 * hostname            hostname                        NULL
 * ipv4                IPv4 address                    interface name (eth0)
 * ipv6                IPv6 address                    interface name (eth0)
 * kernel_release      `uname -r`                      NULL
 * keyboard_indicators caps/num lock indicators        format string (c?n?)
 *                                                     see keyboard_indicators.c
 * keymap              layout (variant) of current     NULL
 *                     keymap
 * load_avg            load average                    NULL
 * netspeed_rx         receive network speed           interface name (wlan0)
 * netspeed_tx         transfer network speed          interface name (wlan0)
 * num_files           number of files in a directory  path
 *                                                     (/home/foo/Inbox/cur)
 * ram_free            free memory in GB               NULL
 * ram_perc            memory usage in percent         NULL
 * ram_total           total memory size in GB         NULL
 * ram_used            used memory in GB               NULL
 * run_command         custom shell command            command (echo foo)
 * swap_free           free swap in GB                 NULL
 * swap_perc           swap usage in percent           NULL
 * swap_total          total swap size in GB           NULL
 * swap_used           used swap in GB                 NULL
 * temp                temperature in degree celsius   sensor file
 *                                                     (/sys/class/thermal/...)
 *                                                     NULL on OpenBSD
 *                                                     thermal zone on FreeBSD
 *                                                     (tz0, tz1, etc.)
 * uid                 UID of current user             NULL
 * up                  interface is running            interface name (eth0)
 * uptime              system uptime                   NULL
 * username            username of current user        NULL
 * vol_perc            OSS/ALSA volume in percent      mixer file (/dev/mixer)
 *                                                     NULL on OpenBSD/FreeBSD
 * wifi_essid          WiFi ESSID                      interface name (wlan0)
 * wifi_perc           WiFi signal in percent          interface name (wlan0)
 */

#define MODULE(name) "/home/martin/dev/suckless-btw/scripts/" name

#if defined(BUILD_DESKTOP)

static const struct arg args[] = {
    /* function          format                                     argument */
    /* Bluetooth & Discord */
    {run_command, "\x09 ^b#16161e^^c#7aa2f7^ %s ", MODULE("discord-status.sh")},

    /* Red (Nuevo bloque) */
    /* Mostramos icono de red y el nombre de la conexión activa */
    {run_command, "\x08 | 󰖩 %s ^d^ ",
     "nmcli -t -f active,ssid dev wifi | grep '^yes' | cut -d: -f2"},

    /* Teclado */
    {run_command, "\x07 ^b#16161e^^c#a9b1d6^  %s ^d^ ",
     "xkb-switch -p | tr '[:lower:]' '[:upper:]'"},

    /* Volumen */
{ run_command, "\x06 ^b#16161e^^c#ff9e64^ %s ", 
  "pactl get-default-sink | grep -Ei 'hyperx|usb' >/dev/null && echo '󰋋' || echo '󰓃'" },
    {run_command, "\x06 | ^b#16161e^^c#ff9e64^  %4s ^d^ ",
     "pamixer --get-volume-human"},

    /* BLOQUE HARDWARE: CPU | RAM | DISCO | TEMP */
    {cpu_perc, "\x05 ^b#16161e^^c#2ac3de^  %s%% ", NULL},
    /* CAMBIADO: AMD Ryzen 5 1600 AF via k10temp (Tdie) */
{run_command, "\x04 |  %s°C ",
     "sensors k10temp-pci-00c3 2>/dev/null | awk '/Tctl|Tdie/{print $2}' | sed 's/+//;s/°C//' | head -n1"},
    {ram_perc, "\x03 |  %s%% ", NULL},
    {disk_perc, "\x02 |  %s%% ^d^ ", "/"},

    /* Fecha y Hora */
    {datetime, "\x01 ^b#16161e^^c#c0caf5^ %s [ ", "%a %b %e"},
    {datetime, "%s ] ^d^", "%H:%M:%S "},
};

#else

static const struct arg args[] = {
  /* function          format                                     argument */
  /* Bluetooth & Discord */
  {run_command, "\x0c ^b#16161e^^c#7aa2f7^ %s ", MODULE("discord-status.sh")},
  {run_command, "\x0b | %s ", MODULE("bluetooth-status.sh")},

  /* Red (Nuevo bloque) */
  /* Mostramos icono de red y el nombre de la conexión activa */
  {run_command, "\x0a | 󰖩 %s ^d^ ",
    "nmcli -t -f active,ssid dev wifi | grep '^yes' | cut -d: -f2"},

  /* Teclado */
  {run_command, "\x09 ^b#16161e^^c#a9b1d6^  %s ^d^ ",
    "xkb-switch -p | tr '[:lower:]' '[:upper:]'"},

  /* Volumen */
  {run_command, "\x08 ^b#16161e^^c#ff9e64^  %4s ^d^ ",
    "pamixer --get-volume-human"},

  /* BLOQUE HARDWARE: CPU | RAM | DISCO | TEMP */
  {cpu_perc, "\x07 ^b#16161e^^c#2ac3de^  %s%% ", NULL},
  /* CAMBIADO: Intel via sysfs en lugar de sensors */
  {run_command, "\x06 |  %s°C ",
    "cat /sys/class/thermal/thermal_zone0/temp | awk '{printf \"%d\", "
      "$1/1000}'"},
  {ram_perc, "\x05 |  %s%% ", NULL},
  {disk_perc, "\x04 |  %s%% ^d^ ", "/"},

  /* Batería */
  {battery_icon, "\x03 ^b#16161e^^c#f7768e^ %s ", "BAT1"},
  {battery_perc, "\x02 %s%% ^d^ ", "BAT1"},

  /* Fecha y Hora */
  {datetime, "\x01 ^b#16161e^^c#c0caf5^ %s [ ", "%a %b %e"},
  {datetime, "%s ] ^d^", "%H:%M:%S "},
};

#endif

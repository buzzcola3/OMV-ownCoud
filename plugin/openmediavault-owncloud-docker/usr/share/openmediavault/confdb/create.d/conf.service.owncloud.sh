#!/usr/bin/env bash
set -e

. /usr/share/openmediavault/scripts/helper-functions

omv_config_add_node "/config/services" "owncloud"
omv_config_add_node_data "/config/services/owncloud" "enable" "0"
omv_config_add_node_data "/config/services/owncloud" "image" "owncloud/server"
omv_config_add_node_data "/config/services/owncloud" "tag" "10.14"
omv_config_add_node_data "/config/services/owncloud" "httpport" "8088"
omv_config_add_node_data "/config/services/owncloud" "dataDir" ""
omv_config_add_node_data "/config/services/owncloud" "adminUser" "admin"
omv_config_add_node_data "/config/services/owncloud" "adminPass" ""

provider "azurerm" {}

resource "azurerm_resource_group" "pb" {
  name     = "${var.prefix}plantbuddy${var.suffix}"
  location = "westeurope"
}

resource "azurerm_storage_account" "func" {
  name                     = "${var.prefix}plantbuddy0func${var.suffix}"
  resource_group_name      = "${azurerm_resource_group.pb.name}"
  location                 = "${azurerm_resource_group.pb.location}"
  account_tier             = "Standard"
  account_replication_type = "LRS"
}

resource "azurerm_app_service_plan" "pb" {
  name                = "${var.prefix}plantbuddy-consumption-plan${var.suffix}"
  location            = "${azurerm_resource_group.pb.location}"
  resource_group_name = "${azurerm_resource_group.pb.name}"
  kind                = "FunctionApp"

  sku {
    tier = "Dynamic"
    size = "Y1"
  }
}

resource "azurerm_function_app" "pb" {
  name                      = "${var.prefix}plantbuddy-func${var.suffix}"
  location                  = "${azurerm_resource_group.pb.location}"
  resource_group_name       = "${azurerm_resource_group.pb.name}"
  app_service_plan_id       = "${azurerm_app_service_plan.pb.id}"
  storage_connection_string = "${azurerm_storage_account.func.primary_connection_string}"

  app_settings {
    TABLE_STORAGE = "${azurerm_storage_account.pb.primary_connection_string}"
    OWM_KEY       = "${var.owm_key}"
    COSMOS_DB     = "AccountEndpoint=${azurerm_cosmosdb_account.pb.endpoint};AccountKey=${azurerm_cosmosdb_account.pb.primary_master_key};"
  }

  provisioner "local-exec" {
    command = "az functionapp deployment source config-local-git --ids ${azurerm_function_app.pb.id}"
  }
}

# databases to persist data into
resource "azurerm_storage_account" "pb" {
  name                     = "${var.prefix}plantbuddy0storage${var.suffix}"
  resource_group_name      = "${azurerm_resource_group.pb.name}"
  location                 = "${azurerm_resource_group.pb.location}"
  account_tier             = "Standard"
  account_replication_type = "LRS"
}

resource "azurerm_cosmosdb_account" "pb" {
  name                      = "${var.prefix}plantbuddy-cosmos${var.suffix}"
  location                  = "${azurerm_resource_group.pb.location}"
  resource_group_name       = "${azurerm_resource_group.pb.name}"
  offer_type                = "Standard"
  kind                      = "GlobalDocumentDB"
  enable_automatic_failover = false

  consistency_policy {
    consistency_level = "BoundedStaleness"
  }

  geo_location {
    location          = "${azurerm_resource_group.pb.location}"
    failover_priority = 0
  }
}

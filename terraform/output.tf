output "git_url" {
  description = "Git endpoint to deploy the function sourcecode to"
  value       = "${azurerm_function_app.pb.name}.scm.azurewebsites.net:443/${azurerm_function_app.pb.name}.git"
}

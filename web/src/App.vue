<template>
  <v-app>
    <v-toolbar color="primary">
      <v-toolbar-title class="title" href="#">Plantbuddy</v-toolbar-title>
    </v-toolbar>
    <router-view/>
  </v-app>
</template>

<script>
export default {
  name: 'App'
}
</script>

<style>
.v-toolbar .title {
  font-weight: bold;
  text-transform: uppercase;
}
</style>

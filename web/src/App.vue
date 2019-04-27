<template>
  <v-app>
    <v-toolbar color="primary">
      <v-toolbar-title class="title" href="#">PlantBuddy</v-toolbar-title>
    </v-toolbar>
    <chart></chart>
  </v-app>
</template>

<script>
import Chart from './components/Chart.vue'
export default {
  name: 'App',
  components: {
    Chart
  }
}
</script>

<style>
.v-toolbar .title {
  font-weight: bold;
  text-transform: uppercase;
}
</style>

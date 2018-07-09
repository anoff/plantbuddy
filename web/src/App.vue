<template>
  <div id="app">
    <b-navbar variant="primary">
      <b-navbar-brand href="#">Plantbuddy</b-navbar-brand>
    </b-navbar>
    <router-view/>
  </div>
</template>

<script>
export default {
  name: 'App'
}
</script>

<style>
.navbar-brand {
  font-weight: bold;
  text-transform: uppercase;
}
</style>

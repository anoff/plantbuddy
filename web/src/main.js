// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'
import BootstrapVue from 'bootstrap-vue'
import firebase from 'firebase/app'
import 'firebase/firestore'
import router from './router'
import './scss/custom.scss'
import 'bootstrap-vue/dist/bootstrap-vue.css'
Vue.config.productionTip = false

Vue.use(BootstrapVue)

firebase.initializeApp({
  projectId: 'plantbuddy-9b971',
  databaseURL: 'https://plantbuddy-9b971.firebaseio.com'
})

export const db = firebase.firestore()
db.settings({timestampsInSnapshots: true})

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  components: { App },
  template: '<App/>'
})

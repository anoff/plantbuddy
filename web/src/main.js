import Vue from 'vue'
import App from './App'
import Vuetify from 'vuetify'
import firebase from 'firebase/app'
import 'firebase/firestore'
import 'material-design-icons-iconfont/dist/material-design-icons.css'
import 'vuetify/dist/vuetify.min.css'
import colors from 'vuetify/es5/util/colors'

Vue.config.productionTip = false

Vue.use(Vuetify, {
  theme: {
    primary: colors.lightGreen.base,
    secondary: colors.green.darken2,
    accent: colors.cyan.darken1,
    error: colors.red.darken4,
    warning: colors.orange.accent3,
    info: colors.lightBlue.base,
    success: colors.green.accent4
  }
})

firebase.initializeApp({
  projectId: 'plantbuddy-9b971',
  databaseURL: 'https://plantbuddy-9b971.firebaseio.com'
})

export const db = firebase.firestore()

new Vue({
  render: h => h(App)
}).$mount('#app')

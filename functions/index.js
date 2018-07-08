const functions = require('firebase-functions')
const admin = require('firebase-admin')
const axios = require('axios')
admin.initializeApp()

exports.newData = functions.https.onRequest((req, res) => {
  // functions.config().header.secret
  // functions.config().owm.key

  if (req.method !== 'POST') {
    return res.status(405).send('Unsupported method 🤷‍')
  }
  const secret = req.get('SECRET')
  if (secret !== functions.config().header.secret) {
    return res.status(401).send('Unauthorized 👮‍ ')
  }
  if (!req.body || ['heartbeat', 'data'].indexOf(req.body.type) === -1) {
    console.log({payload: req.body})
    return res.status(400).send('Invalid payload type 📦')
  }
  const type = req.body.type
  const doc = JSON.parse(JSON.stringify(req.body))
  doc.date = new Date().toISOString()
  admin.firestore().collection(type).add(doc)
    .then(doc => {
      res.send(`Created entry ${doc.id} under /${type}`)
    })
})

// append weather info to sensor reading
exports.fetchWeather = functions.firestore
  .document('data/{entryId}')
  .onCreate((snapshot, context) => {
    const cityId = functions.config().owm.city_id
    const apiKey = functions.config().owm.key

    return axios.get(`https://api.openweathermap.org/data/2.5/weather?id=${cityId}&appid=${apiKey}&units=metric`)
      .then(res => {
        const weather = res.data
        // convert dates to readable strings
        weather.date = new Date(weather.dt * 1000).toISOString()
        weather.sunrise = new Date(weather.sys.sunrise * 1000).toISOString()
        weather.sunset = new Date(weather.sys.sunset * 1000).toISOString()

        return snapshot.ref.set({weather}, {merge: true})
      })
  })

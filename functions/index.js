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
    const data = snapshot.data()
    if (data.aggregate) return null
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

// create hourly aggregates
exports.aggregateHour = functions.firestore
  .document('data/{entryId}')
  .onCreate((snapshot, context) => {
    const data = snapshot.data()
    if (data.aggregate) return null
    const date = data.date
    const hourStart = new Date(date)
    hourStart.setMinutes(0)
    hourStart.setSeconds(0)
    const hourEnd = new Date(date)
    hourEnd.setMinutes(59)
    hourEnd.setSeconds(59)
    const hourId = new Date(date)
    hourId.setMinutes(30)
    hourId.setSeconds(0)
    return admin.firestore().collection('data')
      .where('date', '>=', hourStart.toISOString())
      .where('date', '<=', hourEnd.toISOString())
      .where('id', '==', data.id)
      .get()
      .then(snapshot => {
        const values = snapshot.docs.map(d => Object.assign(d.data(), {_id: d.id}))
        const aggregate = values
          .filter(v => !v.aggregate)
          .reduce((p, c) => {
            p.humidity += c.humidity
            p.moisture += c.moisture
            p.temp += c.temp
            p.weather.clouds.all += c.weather.clouds.all
            p.weather.main.humidity += c.weather.main.humidity
            p.weather.main.pressure += c.weather.main.pressure
            p.weather.main.temp += c.weather.main.temp
            p.weather.main.temp_max = Math.max(c.weather.main.temp_max, p.weather.main.temp_max)
            p.weather.main.temp_min = Math.min(c.weather.main.temp_min, p.weather.main.temp_min)
            p.weather.wind.speed += c.weather.wind.speed
            return p
          }, {
            humidity: 0,
            moisture: 0,
            aggregate: 'hour',
            id: data.id,
            temp: 0,
            weather: {
              clouds: { all: 0 },
              main: { humidity: 0, pressure: 0, temp: 0, temp_max: 0, temp_min: 0 },
              wind: { speed: 0 }
            }
          })
        const numElems = values.filter(v => !v.aggregate).length
        aggregate.humidity /= numElems
        aggregate.moisture /= numElems
        aggregate.temp /= numElems
        aggregate.weather.clouds.all /= numElems
        aggregate.weather.main.humidity /= numElems
        aggregate.weather.main.pressure /= numElems
        aggregate.weather.main.temp /= numElems
        aggregate.weather.wind.speed /= numElems

        return admin.firestore().collection('data').doc(hourId.toISOString()).set(aggregate)
      })
  })

// create daily aggregates
exports.aggregateDay = functions.firestore
  .document('data/{entryId}')
  .onCreate((snapshot, context) => {
    const data = snapshot.data()
    if (data.aggregate !== 'hour') return null
    const date = data.date
    const aggregateStart = new Date(date)
    aggregateStart.setHours(0)
    aggregateStart.setMinutes(0)
    aggregateStart.setSeconds(0)
    const aggregateEnd = new Date(date)
    aggregateEnd.setHours(23)
    aggregateEnd.setMinutes(59)
    aggregateEnd.setSeconds(59)
    const aggregateId = new Date(date)
    aggregateId.setHours(12)
    aggregateId.setMinutes(0)
    aggregateId.setSeconds(0)
    return admin.firestore().collection('data')
      .where('date', '>=', aggregateStart.toISOString())
      .where('date', '<=', aggregateEnd.toISOString())
      .where('id', '==', data.id)
      .where('aggregate', '==', 'hour')
      .get()
      .then(snapshot => {
        const values = snapshot.docs.map(d => Object.assign(d.data(), {_id: d.id}))
        const aggregate = values
          .reduce((p, c) => {
            p.humidity += c.humidity
            p.moisture += c.moisture
            p.temp += c.temp
            p.weather.clouds.all += c.weather.clouds.all
            p.weather.main.humidity += c.weather.main.humidity
            p.weather.main.pressure += c.weather.main.pressure
            p.weather.main.temp += c.weather.main.temp
            p.weather.main.temp_max = Math.max(c.weather.main.temp_max, p.weather.main.temp_max)
            p.weather.main.temp_min = Math.min(c.weather.main.temp_min, p.weather.main.temp_min)
            p.weather.wind.speed += c.weather.wind.speed
            return p
          }, {
            humidity: 0,
            moisture: 0,
            aggregate: 'day',
            id: data.id,
            temp: 0,
            weather: {
              clouds: { all: 0 },
              main: { humidity: 0, pressure: 0, temp: 0, temp_max: 0, temp_min: 0 },
              wind: { speed: 0 }
            }
          })
        const numElems = values.filter(v => !v.aggregate).length
        aggregate.humidity /= numElems
        aggregate.moisture /= numElems
        aggregate.temp /= numElems
        aggregate.weather.clouds.all /= numElems
        aggregate.weather.main.humidity /= numElems
        aggregate.weather.main.pressure /= numElems
        aggregate.weather.main.temp /= numElems
        aggregate.weather.wind.speed /= numElems

        return admin.firestore().collection('data').doc(aggregateId.toISOString()).set(aggregate)
      })
  })

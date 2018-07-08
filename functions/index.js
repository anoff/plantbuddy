const functions = require('firebase-functions')
const admin = require('firebase-admin')
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

// TODO: set up function to fetch weather and append it to each request
// - auth?

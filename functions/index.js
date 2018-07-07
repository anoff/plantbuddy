const functions = require('firebase-functions')
const admin = require('firebase-admin')
admin.initializeApp()

exports.newData = functions.https.onRequest((req, res) => {
  // functions.config().header.secret
  // functions.config().owm.key

  if (req.method !== 'POST') {
    return res.status(403).send('Forbidden 🙅‍ ')
  }
  const secret = req.get('SECRET')
  console.log(functions.config())
  if (secret !== functions.config().header.secret) {
    return res.status(401).send('Unauthorized 👮‍ ')
  }
  admin.firestore().collection('values').add({
    id: 'blup',
    value: 'asdf'
  })
    .then(doc => {
      res.send(`Created entry ${doc.id}`)
    })
})

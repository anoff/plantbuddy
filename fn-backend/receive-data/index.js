module.exports = function (context, req) {
  context.log('JavaScript HTTP trigger function processed a request.')

  if (req.body) {
    context.res = {
      body: 'Hello you'
    }
    context.log(req.body)
  } else {
    context.res = {
      status: 400,
      body: 'Please pass a name on the query string or in the request body'
    }
  }
  context.done()
}

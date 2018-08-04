<template>
  <v-container fluid>
    <v-layout>
      <v-progress-circular
        :width="5"
        color="accent"
        indeterminate
        v-if="loading"
      ></v-progress-circular>
      <v-flex xs12 md8 >
        <line-chart :chartData="dataCollection" :timeFrom="timeFrom" :timeUntil="timeUntil" ref="line"></line-chart>
      </v-flex>
      <v-flex xs12 md4>
        <span>Zoom Level: {{zoomLevel}}</span>
        <v-btn flat icon v-on:click.stop="zoomLevel = Math.max(chart.zoomMin, zoomLevel - 1)">
          <v-icon>zoom_in</v-icon>
        </v-btn>
        <v-btn flat icon v-on:click.stop="zoomLevel = Math.min(chart.zoomMax, zoomLevel + 1)">
          <v-icon>zoom_out</v-icon>
        </v-btn>
        <v-btn flat icon v-on:click.stop="pan(-0.3)">
          <v-icon>chevron_left</v-icon>
        </v-btn>
        <v-btn flat icon v-on:click.stop="pan(0.3)">
          <v-icon>chevron_right</v-icon>
        </v-btn>
      </v-flex>
    </v-layout>
  </v-container>
</template>

<script>
import LineChart from './Linechart.vue'
import {db} from '../main'

export default {
  components: {
    LineChart
  },
  data () {
    return {
      values: [],
      zoomLevel: 2,
      chart: {
        zoomMin: 1,
        zoomMax: 6
      },
      timeUntil: new Date(), // timeFrom is a computed value
      loading: false
    }
  },
  computed: {
    dataCollection: function () {
      const collection = { labels: this.values.map(v => new Date(v.date)), datasets: [] }
      collection.datasets.push({
        label: 'Temperature',
        borderColor: 'rgb(255, 88, 28)',
        backgroundColor: 'rgb(255, 88, 28, 0.2)',
        data: this.values.map(v => v.temp)
      })
      collection.datasets.push({
        label: 'Temperature (OWM)',
        borderColor: 'rgb(247, 173, 27)',
        backgroundColor: 'rgb(247, 173, 27, 0.2)',
        data: this.values.map(v => v.weather.main.temp)
      })
      collection.datasets.push({
        label: 'Cloudiness (OWM)',
        hidden: true,
        borderColor: 'rgb(90, 90, 90)',
        backgroundColor: 'rgb(90, 90, 90, 0.2)',
        data: this.values.map(v => v.weather.clouds.all)
      })
      collection.datasets.push({
        label: 'Humidity',
        hidden: true,
        borderColor: 'rgb(66, 134, 244)',
        backgroundColor: 'rgb(66, 134, 244, 0.2)',
        data: this.values.map(v => v.humidity)
      })
      collection.datasets.push({
        label: 'Humidity (OWM)',
        hidden: true,
        borderColor: 'rgb(27, 188, 247)',
        backgroundColor: 'rgb(27, 188, 247, 0.2)',
        data: this.values.map(v => v.weather.main.humidity)
      })
      collection.datasets.push({
        label: 'Moisture',
        borderColor: 'rgb(163, 79, 34)',
        backgroundColor: 'rgb(163, 79, 34, 0.2)',
        backgroundAlpha: 0.5,
        data: this.values.map(v => 100 - Math.min(100, v.moisture))
      })
      return collection
    },
    timeSpan () {
      let timeSpan // timespan to show on screen [hours]
      switch (this.zoomLevel) {
        case 6:
          timeSpan = 24 * 30 * 3
          break
        case 5:
          timeSpan = 24 * 30
          break
        case 4:
          timeSpan = 24 * 14
          break
        case 3:
          timeSpan = 24 * 7
          break
        case 2:
          timeSpan = 24 * 3
          break
        case 1:
          timeSpan = 24 * 1
          break
      }
      return timeSpan
    },
    timeFrom () {
      return new Date(this.timeUntil - this.timeSpan * 60 * 60 * 1000)
    }
  },
  mounted () {
    this.loadData (this.timeFrom, this.timeUntil)
      .then(val => {
        this.values = val
      })
  },
  methods: {
    loadData (from, until) {
      if (typeof from === 'object') {
        from = from.toISOString()
      }
      if (typeof until === 'object') {
        until = until.toISOString()
      }
      this.loading = true
      let aggregate = 'none'
      if (this.zoomLevel > 1) aggregate = 'hour'
      if (this.zoomLevel > 3) aggregate = 'day'
      // hacky workaround for older data
      let response
      if (new Date(from).toISOString() < '2018-08-04T16:59:02.746Z') {
        if (aggregate === 'none') {
          response = db.collection('data')
          .where('date', '<=', until)
          .where('date', '>=', from)
          .orderBy('date', 'desc')
          .get()
          .then(snapshot => {
            const values = snapshot.docs.map(d => Object.assign(d.data(), {_id: d.id}))
            this.loading = false
            return values.filter(v => v.weather)
          })
        } else {
          response = db.collection('data')
            .where('date', '<=', until)
            .where('date', '>=', from)
            .orderBy('date', 'desc')
            .get()
            .then(snapshot => {
              const values = snapshot.docs
                .map(d => Object.assign(d.data(), {_id: d.id}))
                .filter(d => !d.aggregate || d.aggregate === 'none')
                .filter((d, ix) => ix%(aggregate === 'hour' ? 4 : 6 * 4) === 0) // drop elements to mimic daily/hourly aggregations
              this.loading = false
              return values.filter(v => v.weather)
          })
        }
      } else {
        response = db.collection('data')
          .where('aggregate', '==', aggregate)
          .where('date', '<=', until)
          .where('date', '>=', from)
          .orderBy('date', 'desc')
          .get()
          .then(snapshot => {
            const values = snapshot.docs.map(d => Object.assign(d.data(), {_id: d.id, date: d.id}))
            this.loading = false
            return values.filter(v => v.weather)
        })
      }
      return response
    },
    // pan left/right by given percentage (-1 .. 1)
    pan (percent = 0) {
      const prevFrom = this.timeFrom
      const prevUntil = this.timeUntil
      const timeSpan = this.timeSpan * 3600 * 1000
      this.timeUntil = new Date(this.timeUntil.getTime() + percent * timeSpan)
      if (percent < 0) {
        // TODO: proper logic to identify unknown data points
        this.loadData (this.timeFrom, prevFrom)
          .then(val => this.values = this.values.concat(val))
      }
    }
  },
  watch: {
    zoomLevel () {
      this.loadData (this.timeFrom, this.timeUntil)
      .then(val => this.values = val)
    }
  }
}

</script>

<style scoped>

</style>

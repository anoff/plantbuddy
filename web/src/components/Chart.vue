<template>
<div>
  <div id="progressContainer">
    <v-progress-linear
      height="20"
      color="accent"
      indeterminate
      v-if="loading"
    ></v-progress-linear>
  </div>
  <v-container fluid>
    <v-layout row wrap justify-space-around>
      <v-flex xs12>
        <line-chart :chartData="dataCollection" :timeFrom="timeFrom" :timeUntil="timeUntil" ref="line">
        </line-chart>
        <div id="actionBar"
        v-bind:style="actionBarStyle"
        >
          <v-btn id="zoomlevel" fab large
          v-on:mousedown="startMoveActionBar"
          v-on:touchstart="startMoveActionBar"
          >
          <span>{{zoomLevel}}</span>
          </v-btn>
          <v-btn id="action-top" fab small :color="actionColor" v-on:click.stop="zoomLevel = Math.max(chart.zoomMin, zoomLevel - 1)">
            <v-icon>zoom_in</v-icon>
          </v-btn>
          <v-btn id="action-bottom" fab small :color="actionColor" v-on:click.stop="zoomLevel = Math.min(chart.zoomMax, zoomLevel + 1)">
            <v-icon>zoom_out</v-icon>
          </v-btn>
          <v-btn id="action-left" fab small :color="actionColor" v-on:click.stop="pan(-0.3)">
            <v-icon>chevron_left</v-icon>
          </v-btn>
          <v-btn id="action-right" fab small :color="actionColor" v-on:click.stop="pan(0.3)">
            <v-icon>chevron_right</v-icon>
          </v-btn>
        </div>
      </v-flex>
    </v-layout>
  </v-container>
</div>
</template>

<script>
import LineChart from './Linechart.vue'
import { db } from '../main'

export default {
  components: {
    LineChart
  },
  data () {
    return {
      values: [],
      loadedTimerange: {
        min: null,
        max: null
      },
      zoomLevel: 2,
      chart: {
        zoomMin: 1,
        zoomMax: 6
      },
      timeUntil: new Date(), // timeFrom is a computed value
      loading: false,
      actionBarMoving: false,
      actionBarDOM: document.getElementById('actionBar'),
      actionBarStyle: {
        left: '10%',
        top: '30%'
      },
      actionBarTimer: null
    }
  },
  computed: {
    actionColor: function () {
      return this.actionBarMoving ? 'info' : 'accent'
    },
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
    this.loadData(this.timeFrom, this.timeUntil)
      .then(val => {
        this.values = val
      })
    window.addEventListener('mousemove', this.moveActionBar)
    window.addEventListener('touchmove', this.moveActionBar)
    window.addEventListener('mouseup', this.stopMoveActionBar)
    window.addEventListener('touchend', this.stopMoveActionBar)
    window.addEventListener('touchcancel', this.stopMoveActionBar)
  },
  methods: {
    loadData (from, until) {
      if (typeof from === 'object') {
        from = from.toISOString()
      }
      if (typeof until === 'object') {
        until = until.toISOString()
      }
      if (from >= this.loadedTimerange.min && until <= this.loadedTimerange.max) return Promise.resolve([])
      if (until > this.loadedTimerange.min && from < this.loadedTimerange.min) until = this.loadedTimerange.min
      if (from < this.loadedTimerange.max && until > this.loadedTimerange.max) from = this.loadedTimerange.max
      this.loading = true
      let aggregate = 'none'
      if (this.zoomLevel > 1) aggregate = 'hour'
      if (this.zoomLevel > 3) aggregate = 'day'
      // hacky workaround for older data
      let response
      if (aggregate === 'none') {
        response = db.collection('data')
          .where('date', '<=', until)
          .where('date', '>=', from)
          .orderBy('date', 'desc')
          .get()
          .then(snapshot => {
            const values = snapshot.docs
              .map(d => Object.assign(d.data(), { _id: d.id }))
              .filter(d => !d.aggregate || d.aggregate === 'none')
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
              .map(d => Object.assign(d.data(), { _id: d.id }))
              .filter(d => !d.aggregate || d.aggregate === 'none')
              .filter((d, ix) => ix % (aggregate === 'hour' ? 4 : 6 * 4) === 0) // drop elements to mimic daily/hourly aggregations
            this.loading = false
            return values.filter(v => v.weather)
          })
      }
      if (!this.loadedTimerange.min || from < this.loadedTimerange.min) {
        this.loadedTimerange.min = from
      }
      if (!this.loadedTimerange.max || until > this.loadedTimerange.max) {
        this.loadedTimerange.max = until
      }
      return response
    },
    // pan left/right by given percentage (-1 .. 1)
    pan (percent = 0) {
      const timeSpan = this.timeSpan * 3600 * 1000
      this.timeUntil = new Date(this.timeUntil.getTime() + percent * timeSpan)
      this.loadData(this.timeFrom, this.timeUntil)
        .then(val => { this.values = this.values.concat(val) })
    },
    startMoveActionBar () {
      this.actionBarTimer = setTimeout(() => { this.actionBarMoving = true }, 600)
    },
    stopMoveActionBar () {
      clearTimeout(this.actionBarTimer)
      this.actionBarTimer = null
      this.actionBarMoving = false
    },
    moveActionBar (event) {
      if (this.actionBarMoving) {
        const x = event.x || event.touches[0].pageX
        const y = event.y || event.touches[0].pageY
        this.actionBarStyle.left = x - 60 + 'px'
        this.actionBarStyle.top = y - 60 + 'px'
      }
    }
  },
  watch: {
    zoomLevel () {
      this.loadedTimerange = { min: null, max: null }
      this.loadData(this.timeFrom, this.timeUntil)
        .then(val => { this.values = val })
    }
  }
}

</script>

<style scoped>

#progressContainer {
  min-height: 20px;
}
.v-progress-linear {
  margin: 0;
}
.container {
  padding-top: 0;
}

#actionBar {
  width: 145px;
  height: 140px;
  position: absolute;
}
#action-top {
  top: 0px;
  left: 47px;
}
#action-bottom {
  bottom: 0px;
  left: 47px;
}
#action-left {
  top: 45px;
  left: 0px;
}
#action-right {
  top: 45px;
  right: 0px;
}
#actionBar .v-btn {
  position: absolute;
}
#zoomlevel {
  position: absolute;
  top: 30px;
  left: 30px;
}
#zoomlevel span {
  color: gray;
}
</style>

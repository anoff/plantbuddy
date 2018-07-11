<template>
  <v-container fluid>
    <v-layout>
      <v-progress-circular
        :width="5"
        color="accent"
        indeterminate
        v-if="!loaded"
      ></v-progress-circular>
      <v-flex xs12 md8 v-if="loaded">
        <line-chart :chartData="dataCollection" :chartOptions="chartOptions" :zoomLevel="zoomLevel" ref="line"></line-chart>
      </v-flex>
      <v-flex xs12 md4>
        <span>Zoom Level: {{zoomLevel}}</span>
        <v-btn flat icon v-on:click.stop="zoomLevel = Math.max(1, zoomLevel - 1)">
          <v-icon>zoom_in</v-icon>
        </v-btn>
        <v-btn flat icon v-on:click.stop="zoomLevel = Math.min(6, zoomLevel + 1)">
          <v-icon>zoom_out</v-icon>
        </v-btn>
        <v-btn flat icon v-on:click.stop="$refs.line.pan(-0.3)">
          <v-icon>chevron_left</v-icon>
        </v-btn>
        <v-btn flat icon v-on:click.stop="$refs.line.pan(0.3)">
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
      pan: 0,
      loaded: false,
      chartOptions: {
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: true,
              suggestedMax: 100
            },
            gridLines: {
              display: true
            }
          }],
          xAxes: [ {
            type: 'time',
            time: {
              displayFormats: {
                minute: 'HH:mm',
                hour: 'HH:mm'
              },
              tooltipFormat: 'YYYY-MM-DD HH:mm'
            },
            gridLines: {
              display: false
            }
          }]
        },
        tooltips: {
          backgroundColor: 'rgba(255, 255, 255, 0.8)',
          titleFontColor: '#333',
          displayColors: false,
          bodyFontColor: '#000',
          footerFontColor: '#333'
        },
        legend: {
          display: true
        },
        responsive: true,
        maintainAspectRatio: false,
        pan: {
          enabled: true,
          mode: 'xy'
        }
      }
    }
  },
  computed: {
    dataCollection: function () {
      const collection = { labels: this.values.map(v => new Date(v.date)), datasets: [] }
      const datasetStyle = {

      }
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
    }
  },
  mounted () {
    return db.collection('data').orderBy('date', 'desc').get()
    .then(snapshot => {
      const values = snapshot.docs.map(d => Object.assign(d.data(), {_id: d.id}))
      this.values = values
      .filter(v => v.weather)
      .filter((e, ix) => ix%3 === 0)
      this.loaded = true
    })
  }
}

</script>

<style scoped>

</style>

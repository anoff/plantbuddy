<template>
  <b-container>
    <line-chart :chartData="dataCollection" :chartOptions="chartOptions"></line-chart>
  </b-container>
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
      loaded: false,
      chartOptions: {
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: true
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
        maintainAspectRatio: false
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
        label: 'Humidity',
        borderColor: 'rgb(66, 134, 244)',
        backgroundColor: 'rgb(66, 134, 244, 0.2)',
        data: this.values.map(v => v.humidity)
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
      this.values = values.filter((e, ix) => ix%3 === 0)
      this.loaded = true
    })
  }
}

</script>

<style scoped>

</style>

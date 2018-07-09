<template>
  <div class="chart">
     <line-chart :data="datacollection" :options="{responsive: true, maintainAspectRatio: false}"></line-chart>
  </div>
</template>

<script>
import LineChart from './Linechart.vue'

export default {
  components: {
    LineChart
  },
  data () {
    return {
      datacollection: {
        labels: ['January', 'February'],
        datasets: [
          {
            label: 'Data One',
            backgroundColor: '#f87979',
            data: [40, 20]
          }
        ]
      }
    }
  }
}

</script>

<style scoped>

</style>

import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import questApi from '../api/quests.js'

export const useQuestStore = defineStore('quests', () => {
  const dailyQuests = ref([])
  const weeklyQuests = ref([])
  const legendaryQuests = ref([])
  const points = ref(0)
  const dailyCompleted = ref(0)
  const dailyTotal = ref(4)
  const lastReset = ref('')
  const loading = ref(false)
  const error = ref('')

  const dailyProgress = computed(() => {
    if (dailyTotal.value === 0) return 0
    return Math.round((dailyCompleted.value / dailyTotal.value) * 100)
  })

  const completedQuests = computed(() => {
    const all = [...dailyQuests.value, ...weeklyQuests.value, ...legendaryQuests.value]
    return all.filter(q => q.status === 'completed')
  })

  const claimedQuests = computed(() => {
    const all = [...dailyQuests.value, ...weeklyQuests.value, ...legendaryQuests.value]
    return all.filter(q => q.status === 'claimed')
  })

  async function fetchQuests() {
    loading.value = true
    error.value = ''
    try {
      const res = await questApi.getQuests()
      if (res.data.code === 200) {
        const data = res.data.data
        dailyQuests.value = data.daily?.quests || []
        dailyCompleted.value = data.daily?.completed_count || 0
        dailyTotal.value = data.daily?.total_count || 4
        lastReset.value = data.daily?.reset_at || ''
        weeklyQuests.value = data.weekly?.quests || []
        legendaryQuests.value = data.legendary?.quests || []
        points.value = data.total_points || 0
      }
    } catch (e) {
      error.value = e.response?.data?.message || '获取任务列表失败'
    } finally {
      loading.value = false
    }
  }

  async function reportProgress(questKey, increment = 1) {
    try {
      const res = await questApi.reportProgress(questKey, increment)
      if (res.data.code === 200) {
        const data = res.data.data
        // 更新本地状态
        const allQuests = [...dailyQuests.value, ...weeklyQuests.value, ...legendaryQuests.value]
        const quest = allQuests.find(q => q.id === data.quest_id)
        if (quest) {
          quest.status = data.status
          quest.current_progress = data.current_progress
          if (data.completed_at) quest.completed_at = data.completed_at
        }
        // 如果是每日任务完成，更新计数
        const dailyQuest = dailyQuests.value.find(q => q.id === data.quest_id)
        if (dailyQuest && data.status === 'completed') {
          dailyCompleted.value = Math.min(dailyCompleted.value + 1, dailyTotal.value)
        }
        return data
      }
    } catch (e) {
      error.value = e.response?.data?.message || '上报进度失败'
      throw e
    }
  }

  async function claimReward(questId) {
    try {
      const res = await questApi.claimReward(questId)
      if (res.data.code === 200) {
        const data = res.data.data
        points.value = data.total_points
        // 更新本地状态
        const allQuests = [...dailyQuests.value, ...weeklyQuests.value, ...legendaryQuests.value]
        const quest = allQuests.find(q => q.id === questId)
        if (quest) {
          quest.status = 'claimed'
          quest.claimed_at = data.claimed_at
        }
        return data
      }
    } catch (e) {
      error.value = e.response?.data?.message || '领取奖励失败'
      throw e
    }
  }

  async function fetchPoints() {
    try {
      const res = await questApi.getPoints()
      if (res.data.code === 200) {
        points.value = res.data.data.points
      }
    } catch (e) {
      // 静默失败
    }
  }

  return {
    dailyQuests,
    weeklyQuests,
    legendaryQuests,
    points,
    dailyCompleted,
    dailyTotal,
    lastReset,
    loading,
    error,
    dailyProgress,
    completedQuests,
    claimedQuests,
    fetchQuests,
    reportProgress,
    claimReward,
    fetchPoints,
  }
})

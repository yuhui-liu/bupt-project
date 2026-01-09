import { type ClassValue, clsx } from "clsx"
import { twMerge } from "tailwind-merge"
import { UPLOAD_BASE_URL } from './constants'

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}

/**
 * Convert relative file path from backend to absolute URL
 * @param relativePath - Relative path like "needs/xxx/images/yyy.png"
 * @returns Absolute URL like "http://localhost:8080/uploads/needs/xxx/images/yyy.png"
 */
export function getFileUrl(relativePath: string | null | undefined): string {
  if (!relativePath) return ''
  // If already absolute URL, return as-is
  if (relativePath.startsWith('http://') || relativePath.startsWith('https://')) {
    return relativePath
  }
  // Build absolute URL
  return `${UPLOAD_BASE_URL}/${relativePath}`
}

export function formatDate(date: string | Date): string {
  const d = typeof date === 'string' ? new Date(date) : date
  return d.toLocaleDateString('zh-CN', {
    year: 'numeric',
    month: '2-digit',
    day: '2-digit',
    hour: '2-digit',
    minute: '2-digit'
  })
}

export function formatFileSize(bytes: number): string {
  if (bytes === 0) return '0 Bytes'
  const k = 1024
  const sizes = ['Bytes', 'KB', 'MB', 'GB']
  const i = Math.floor(Math.log(bytes) / Math.log(k))
  return Math.round((bytes / Math.pow(k, i)) * 100) / 100 + ' ' + sizes[i]
}

export function truncate(str: string, length: number): string {
  if (str.length <= length) return str
  return str.substring(0, length) + '...'
}

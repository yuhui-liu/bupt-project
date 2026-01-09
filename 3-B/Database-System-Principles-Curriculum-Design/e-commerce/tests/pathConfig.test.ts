import { DEFAULT_PATH_CONFIG, getEnvironmentPaths, validateExportPath, generatePathSuggestions } from '../src/lib/pathConfig';

describe('pathConfig module', () => {
  describe('DEFAULT_PATH_CONFIG', () => {
    it('should be a non-empty array with default entries', () => {
      expect(Array.isArray(DEFAULT_PATH_CONFIG)).toBe(true);
      expect(DEFAULT_PATH_CONFIG.length).toBeGreaterThan(0);
      const first = DEFAULT_PATH_CONFIG[0];
      expect(first).toHaveProperty('value');
      expect(first).toHaveProperty('label');
    });
  });

  describe('getEnvironmentPaths', () => {
    const origEnv = process.env;

    beforeEach(() => {
      process.env = { ...origEnv };
    });
    afterEach(() => {
      process.env = origEnv;
    });

    it('includes custom path when NEXT_PUBLIC_EXPORT_PATH is set', () => {
      process.env.NEXT_PUBLIC_EXPORT_PATH = '/custom';
      const paths = getEnvironmentPaths();
      expect(paths.some(p => p.value === '/custom')).toBe(true);
    });

    it('includes development paths when NODE_ENV is development', () => {
      (process.env as Record<string, string>).NODE_ENV = 'development';
      const paths = getEnvironmentPaths();
      expect(paths.some(p => p.category === 'user')).toBe(true);
    });
  });

  describe('validateExportPath', () => {
    it('validates empty path', () => {
      expect(validateExportPath('')).toEqual({ valid: true });
    });

    it('rejects relative paths', () => {
      expect(validateExportPath('relative/path').valid).toBe(false);
    });

    it('rejects paths with illegal characters', () => {
      expect(validateExportPath('/path/with<illegal>').valid).toBe(false);
    });

    it('rejects overly long paths', () => {
      const longPath = '/' + 'a'.repeat(256);
      expect(validateExportPath(longPath).valid).toBe(false);
    });
  });

  describe('generatePathSuggestions', () => {
    it('returns a maximum of 5 suggestions including common patterns', () => {
      const history = ['/data/files/report.csv'];
      const suggestions = generatePathSuggestions(history);
      expect(Array.isArray(suggestions)).toBe(true);
      expect(suggestions.length).toBeLessThanOrEqual(5);
      expect(suggestions).toEqual(expect.arrayContaining(['/data']));
      expect(suggestions).toEqual(expect.arrayContaining(['/exports']));
    });
  });
});

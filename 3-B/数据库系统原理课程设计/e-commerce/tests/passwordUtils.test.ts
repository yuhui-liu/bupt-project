import { checkPasswordStrength, passwordValidationRules } from '../src/lib/passwordUtils';

describe('checkPasswordStrength', () => {
  it('returns weak for empty password', () => {
    const result = checkPasswordStrength('');
    expect(result.score).toBe(0);
    expect(result.level).toBe('weak');
    expect(result.text).toBe('请输入密码');
    expect(result.percentage).toBe(0);
  });

  it('returns strong for password with all criteria', () => {
    const result = checkPasswordStrength('Abcde123!');
    expect(result.score).toBe(4);
    expect(result.level).toBe('strong');
    expect(result.percentage).toBe(100);
  });

  it('returns medium for password meets two criteria', () => {
    const result = checkPasswordStrength('Abcdefgh');
    expect(result.score).toBe(2);
    expect(result.level).toBe('medium');
    expect(result.percentage).toBe(60);
  });
});

describe('passwordValidationRules', () => {
  const validator = passwordValidationRules.find(rule => 'validator' in rule)?.validator!;

  it('rejects when missing criteria', async () => {
    await expect(validator(null, 'abcdefg')).rejects.toThrow('密码必须包含大小写字母和数字！');
  });

  it('resolves when meets criteria', async () => {
    await expect(validator(null, 'Abcdefg1')).resolves.toBeUndefined();
  });
});

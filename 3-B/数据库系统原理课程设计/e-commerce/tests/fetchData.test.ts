import { register, login } from '../src/lib/fetchData';

describe('fetchData functions', () => {
  beforeEach(() => {
    // mock fetch
    global.fetch = jest.fn().mockResolvedValue({
      ok: true,
      json: async () => ({})
    } as any);
  });

  it('register calls fetch with correct url and payload', async () => {
    const email = 'a@b.com';
    const username = 'user';
    const password = 'pass';
    const captcha = 'code';
    await register(email, username, password, captcha);
    expect(global.fetch).toHaveBeenCalledWith(
      `${process.env.NEXT_PUBLIC_API_URL}/api/register`,
      expect.objectContaining({
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, username, password, captcha })
      })
    );
  });

  it('login calls fetch with correct url', async () => {
    const username = 'user';
    const password = 'pass';
    await login(username, password);
    expect(global.fetch).toHaveBeenCalledWith(
      `${process.env.NEXT_PUBLIC_API_URL}/api/login`,
      expect.objectContaining({ method: 'POST' })
    );
  });
});

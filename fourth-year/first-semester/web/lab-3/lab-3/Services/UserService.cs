using lab_3.Models;

namespace lab_3.Services;

public sealed class UserService
{
    private readonly Dictionary<Guid, User> _users = new();

    public async Task<User> CreateUserAsync(User user)
    {
        user.Id = Guid.CreateVersion7();
        _users.Add(user.Id, user);
        return await Task.FromResult(user);
    }

    public async Task<IEnumerable<User>> GetUsersAsync()
    {
        return await Task.FromResult(_users.Values);
    }

    public async Task<User?> GetUserById(Guid id)
    {
        return await Task.FromResult(_users.GetValueOrDefault(id));
    }
}
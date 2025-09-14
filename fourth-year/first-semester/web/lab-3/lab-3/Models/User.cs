namespace lab_3.Models;

public sealed class User
{
    public Guid Id { get; set; }
    public string Username { get; set; }
    public string Name { get; set; }
    public string Email { get; set; }

    public User(string username, string name, string email)
    {
        Username = username;
        Name = name;
        Email = email;
    }
}
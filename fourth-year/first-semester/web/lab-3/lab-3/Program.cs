using lab_3.Models;
using lab_3.Services;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddOpenApi();
builder.Services.AddControllers();

builder.Services.AddSingleton<MessageService>();
builder.Services.AddSingleton<UserService>();

var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.MapOpenApi();
}

var userService = app.Services.GetRequiredService<UserService>();

var user1 = new User("user1", "User 1", "user1@mail.com");
var user2 = new User("user2", "User 2", "user2@mail.com");
var user3 = new User("user3", "User 3", "user3@mail.com");

await userService.CreateUserAsync(user1);
await userService.CreateUserAsync(user2);
await userService.CreateUserAsync(user3);

var messageService = app.Services.GetRequiredService<MessageService>();

await messageService.SendMessage(user1, user2, "Hello!");
await messageService.SendMessage(user2, user1, "Hi!");

await messageService.SendMessage(user1, user3, "Hi! What's up?");
await messageService.SendMessage(user3, user1, "Hi, I'm good, wbu?");

app.UseHttpsRedirection();
app.MapControllers();
app.Run();
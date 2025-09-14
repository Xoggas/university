using lab_3.Models;

namespace lab_3.Services;

public sealed class MessageService
{
    private readonly Dictionary<User, List<Message>> _sentBy = [];
    private readonly Dictionary<User, List<Message>> _sentTo = [];

    public async Task<Message> SendMessage(User sender, User receiver, string content)
    {
        var message = new Message(Guid.CreateVersion7(), sender, receiver, content);

        if (_sentBy.ContainsKey(sender) is false)
        {
            _sentBy[sender] = [];
        }
        
        if (_sentTo.ContainsKey(receiver) is false)
        {
            _sentTo[receiver] = [];
        }
        
        _sentBy[sender].Add(message);
        _sentTo[receiver].Add(message);

        return await Task.FromResult(message);
    }

    public async Task<IEnumerable<Message>> GetMessagesSentByTo(User sender, User receiver)
    {
        return await Task.FromResult(_sentBy[sender].Where(x => x.SentTo == receiver));
    }
}
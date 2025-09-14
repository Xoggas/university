namespace lab_3.Models;

public sealed class Message
{
    public Guid Id { get; set; }
    public User SentBy { get; set; }
    public User SentTo { get; set; }
    public string Content { get; set; }

    public Message(Guid id, User sentBy, User sentTo, string content)
    {
        Id = id;
        SentBy = sentBy;
        SentTo = sentTo;
        Content = content;
    }
}
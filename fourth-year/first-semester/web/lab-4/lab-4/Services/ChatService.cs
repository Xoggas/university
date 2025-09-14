using AutoMapper;
using CSharpFunctionalExtensions;
using Microsoft.EntityFrameworkCore;
using Talksy.Api.Data;
using Talksy.Api.Dtos.Chat;
using Talksy.Api.Dtos.Message;
using Talksy.Api.Models;

namespace Talksy.Api.Services;

public interface IChatService
{
    Task<Result<IEnumerable<ChatDto>>> FindChatsForUserAsync(Guid userId);
    Task<Result<ChatDto>> CreateChatAsync(Guid user1Id, Guid user2Id);
    Task<Result<IEnumerable<MessageDto>>> GetMessagesFromChatAsync(Guid chatId);
    Task<Result<MessageDto>> SendMessageAsync(MessageCreateDto dto);
}

public sealed class ChatService : IChatService
{
    private readonly AppDbContext _dbContext;
    private readonly IMapper _mapper;

    public ChatService(AppDbContext dbContext, IMapper mapper)
    {
        _dbContext = dbContext;
        _mapper = mapper;
    }

    public async Task<Result<IEnumerable<ChatDto>>> FindChatsForUserAsync(Guid userId)
    {
        var chatModels = await _dbContext.Chats
            .Where(x => x.ChatMember1.Id == userId || x.ChatMember2.Id == userId)
            .Include(x => x.ChatMember1)
            .Include(x => x.ChatMember2)
            .ToListAsync();

        if (chatModels.Count == 0)
        {
            return Result.Failure<IEnumerable<ChatDto>>("There are no chats");
        }

        var chatDtos = _mapper.Map<IEnumerable<ChatDto>>(chatModels);

        return Result.Success(chatDtos);
    }

    public async Task<Result<ChatDto>> CreateChatAsync(Guid user1Id, Guid user2Id)
    {
        var user1 = await _dbContext.Users.FirstOrDefaultAsync(x => x.Id == user1Id);
        var user2 = await _dbContext.Users.FirstOrDefaultAsync(x => x.Id == user2Id);

        if (user1 is null || user2 is null)
        {
            return Result.Failure<ChatDto>("One of the users doesn't exist");
        }

        var chatExists = _dbContext.Chats.Any(x =>
            x.ChatMember1.Id == user1Id && x.ChatMember2.Id == user2Id ||
            x.ChatMember1.Id == user2Id && x.ChatMember2.Id == user1Id);

        if (chatExists)
        {
            return Result.Failure<ChatDto>("There is already a chat");
        }

        var chatModel = new Chat
        {
            Id = Guid.CreateVersion7(),
            ChatMember1 = user1,
            ChatMember2 = user2,
        };

        await _dbContext.Chats.AddAsync(chatModel);
        await _dbContext.SaveChangesAsync();

        var chatDto = _mapper.Map<ChatDto>(chatModel);

        return Result.Success(chatDto);
    }

    public async Task<Result<IEnumerable<MessageDto>>> GetMessagesFromChatAsync(Guid chatId)
    {
        var messages = await _dbContext.Messages
            .Where(x => x.Chat.Id == chatId)
            .Include(x => x.Chat)
            .Include(x => x.Sender)
            .ToListAsync();

        if (messages.Count == 0)
        {
            return Result.Failure<IEnumerable<MessageDto>>("There are no messages");
        }

        var messagesDto = _mapper.Map<IEnumerable<MessageDto>>(messages);

        return Result.Success(messagesDto);
    }

    public async Task<Result<MessageDto>> SendMessageAsync(MessageCreateDto dto)
    {
        var sender = await _dbContext.Users.FirstOrDefaultAsync(x => x.Id == dto.SenderId);
        if (sender is null)
        {
            return Result.Failure<MessageDto>("Sender doesn't exist");
        }

        var chat = await _dbContext.Chats.FirstOrDefaultAsync(x => x.Id == dto.ChatId);
        if (chat is null)
        {
            return Result.Failure<MessageDto>("Chat doesn't exit");
        }

        var messageModel = new Message
        {
            Id = Guid.CreateVersion7(),
            Chat = chat,
            Sender = sender,
            Content = dto.Content
        };

        await _dbContext.Messages.AddAsync(messageModel);
        await _dbContext.SaveChangesAsync();

        var messageDto = _mapper.Map<MessageDto>(messageModel);

        return Result.Success(messageDto);
    }
}
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Swashbuckle.AspNetCore.Annotations;
using Talksy.Api.Dtos.Chat;
using Talksy.Api.Dtos.Message;
using Talksy.Api.Services;
using UserModel = Talksy.Api.Models.User;

namespace Talksy.Api.Controllers;

/// <summary>
/// Управление чатами и сообщениями.
/// </summary>
[ApiController]
[Route("api/chat")]
public sealed class ChatController : ControllerBase
{
    private readonly IChatService _chatService;

    public ChatController(IChatService chatService)
    {
        _chatService = chatService;
    }

    private Guid GetUserIdFromJwt()
    {
        var userIdRaw = User.FindFirst(UserModel.JwtUserIdClaimName)!.Value;
        var userId = Guid.Parse(userIdRaw);
        return userId;
    }

    /// <summary>
    /// Получить список чатов текущего пользователя.
    /// </summary>
    /// <returns>Список чатов.</returns>
    [HttpGet]
    [Authorize]
    [SwaggerOperation(Summary = "Получить чаты пользователя",
        Description = "Возвращает список чатов для текущего авторизованного пользователя.")]
    [ProducesResponseType(typeof(IEnumerable<ChatDto>), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<IEnumerable<ChatDto>>> GetChatsAsync()
    {
        var userId = GetUserIdFromJwt();
        var result = await _chatService.FindChatsForUserAsync(userId);

        if (result.IsFailure)
        {
            return NotFound();
        }

        var chatDtos = result.Value;
        return Ok(chatDtos);
    }


    /// <summary>
    /// Создать новый чат между текущим пользователем и другим пользователем.
    /// </summary>
    /// <param name="receiverId">Id пользователя, с которым создаётся чат.</param>
    /// <returns>Информация о созданном чате.</returns>
    [HttpPost("{receiverId:guid}")]
    [Authorize]
    [SwaggerOperation(Summary = "Создать чат",
        Description = "Создает чат между текущим пользователем и указанным получателем.")]
    [ProducesResponseType(typeof(ChatDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<ChatDto>> CreateChatAsync(Guid receiverId)
    {
        var senderId = GetUserIdFromJwt();
        var result = await _chatService.CreateChatAsync(senderId, receiverId);

        if (result.IsFailure)
        {
            return BadRequest(result.Error);
        }

        var chatDto = result.Value;
        return Ok(chatDto);
    }

    /// <summary>
    /// Получить все сообщения из указанного чата.
    /// </summary>
    /// <param name="chatId">Id чата.</param>
    /// <returns>Список сообщений.</returns>
    [HttpGet("message/{chatId:guid}")]
    [Authorize]
    [SwaggerOperation(Summary = "Получить сообщения чата",
        Description = "Возвращает список сообщений из указанного чата.")]
    [ProducesResponseType(typeof(IEnumerable<MessageDto>), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<IEnumerable<MessageDto>>> GetMessagesFromChatAsync(Guid chatId)
    {
        var result = await _chatService.GetMessagesFromChatAsync(chatId);

        if (result.IsFailure)
        {
            return BadRequest(result.Error);
        }

        var messageDtos = result.Value;
        return Ok(messageDtos);
    }

    /// <summary>
    /// Отправить сообщение в чат.
    /// </summary>
    /// <param name="dto">Данные для создания сообщения.</param>
    /// <returns>Отправленное сообщение.</returns>
    [HttpPost("message")]
    [Authorize]
    [SwaggerOperation(Summary = "Отправить сообщение", Description = "Отправляет новое сообщение в чат.")]
    [ProducesResponseType(typeof(MessageDto), StatusCodes.Status200OK)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<MessageDto>> SendMessageAsync(MessageCreateDto dto)
    {
        var result = await _chatService.SendMessageAsync(dto);

        if (result.IsFailure)
        {
            return BadRequest(result.Error);
        }

        var messageDto = result.Value;
        return Ok(messageDto);
    }
}
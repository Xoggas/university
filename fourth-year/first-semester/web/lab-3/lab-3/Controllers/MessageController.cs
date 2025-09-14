using lab_3.Models;
using lab_3.Services;
using Microsoft.AspNetCore.Mvc;

namespace lab_3.Controllers;

[ApiController]
[Route("api/message")]
public sealed class MessageController : ControllerBase
{
    private readonly MessageService _messageService;
    private readonly UserService _userService;

    public MessageController(MessageService messageService, UserService userService)
    {
        _messageService = messageService;
        _userService = userService;
    }

    [HttpGet("{sentBy:guid}/{sentTo:guid}")]
    public async Task<ActionResult<IEnumerable<Message>>> GetMessageSendByToAsync(Guid sentBy, Guid sentTo)
    {
        var sender = await _userService.GetUserById(sentBy);
        var receiver = await _userService.GetUserById(sentTo);

        if (sender is null || receiver is null)
        {
            return NotFound();
        }

        var messages = await _messageService.GetMessagesSentByTo(sender, receiver);

        return Ok(messages);
    }
}
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Talksy.Api.Migrations
{
    /// <inheritdoc />
    public partial class FixedNamingOfChatModel : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropForeignKey(
                name: "FK_Chats_Users_ChatMemer1Id",
                table: "Chats");

            migrationBuilder.DropForeignKey(
                name: "FK_Chats_Users_ChatMemer2Id",
                table: "Chats");

            migrationBuilder.RenameColumn(
                name: "ChatMemer2Id",
                table: "Chats",
                newName: "ChatMember2Id");

            migrationBuilder.RenameColumn(
                name: "ChatMemer1Id",
                table: "Chats",
                newName: "ChatMember1Id");

            migrationBuilder.RenameIndex(
                name: "IX_Chats_ChatMemer2Id",
                table: "Chats",
                newName: "IX_Chats_ChatMember2Id");

            migrationBuilder.RenameIndex(
                name: "IX_Chats_ChatMemer1Id",
                table: "Chats",
                newName: "IX_Chats_ChatMember1Id");

            migrationBuilder.AddForeignKey(
                name: "FK_Chats_Users_ChatMember1Id",
                table: "Chats",
                column: "ChatMember1Id",
                principalTable: "Users",
                principalColumn: "Id");

            migrationBuilder.AddForeignKey(
                name: "FK_Chats_Users_ChatMember2Id",
                table: "Chats",
                column: "ChatMember2Id",
                principalTable: "Users",
                principalColumn: "Id");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropForeignKey(
                name: "FK_Chats_Users_ChatMember1Id",
                table: "Chats");

            migrationBuilder.DropForeignKey(
                name: "FK_Chats_Users_ChatMember2Id",
                table: "Chats");

            migrationBuilder.RenameColumn(
                name: "ChatMember2Id",
                table: "Chats",
                newName: "ChatMemer2Id");

            migrationBuilder.RenameColumn(
                name: "ChatMember1Id",
                table: "Chats",
                newName: "ChatMemer1Id");

            migrationBuilder.RenameIndex(
                name: "IX_Chats_ChatMember2Id",
                table: "Chats",
                newName: "IX_Chats_ChatMemer2Id");

            migrationBuilder.RenameIndex(
                name: "IX_Chats_ChatMember1Id",
                table: "Chats",
                newName: "IX_Chats_ChatMemer1Id");

            migrationBuilder.AddForeignKey(
                name: "FK_Chats_Users_ChatMemer1Id",
                table: "Chats",
                column: "ChatMemer1Id",
                principalTable: "Users",
                principalColumn: "Id");

            migrationBuilder.AddForeignKey(
                name: "FK_Chats_Users_ChatMemer2Id",
                table: "Chats",
                column: "ChatMemer2Id",
                principalTable: "Users",
                principalColumn: "Id");
        }
    }
}

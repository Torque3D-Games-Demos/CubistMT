
$START_VIEW_DELAY = $pref::Cube::delayForClients-500;

// ----------------------------------------------------------------------------
// CubeMTGame
// ----------------------------------------------------------------------------
// Depends on methods found in gameCore.cs.  Those added here are specific to
// this game type and/or over-ride the "default" game functionaliy.
//
// The desired Game Type must be added to each mission's LevelInfo object.
//   - gameType = "Deathmatch";
// If this information is missing then the GameCore will default to Deathmatch.
// ----------------------------------------------------------------------------

function CubeMTGame::onMissionLoaded(%game)
{
   $Server::MissionType = "CubeMT";
   parent::onMissionLoaded(%game);
}

function CubeMTGame::initGameVars(%game)
{
   //-----------------------------------------------------------------------------
   // What kind of "player" is spawned is either controlled directly by the
   // SpawnSphere or it defaults back to the values set here. This also controls
   // which SimGroups to attempt to select the spawn sphere's from by walking down
   // the list of SpawnGroups till it finds a valid spawn object.
   // These override the values set in core/scripts/server/spawn.cs
   //-----------------------------------------------------------------------------
   $Game::defaultPlayerClass = "Player";
   $Game::defaultPlayerDataBlock = "DefaultPlayerData";
   $Game::defaultPlayerSpawnGroups = "PlayerSpawnPoints PlayerDropPoints";

   //-----------------------------------------------------------------------------
   // What kind of "camera" is spawned is either controlled directly by the
   // SpawnSphere or it defaults back to the values set here. This also controls
   // which SimGroups to attempt to select the spawn sphere's from by walking down
   // the list of SpawnGroups till it finds a valid spawn object.
   // These override the values set in core/scripts/server/spawn.cs
   //-----------------------------------------------------------------------------
   $Game::defaultCameraClass = "Camera";
   $Game::defaultCameraDataBlock = "Observer";
   $Game::defaultCameraSpawnGroups = "CameraSpawnPoints PlayerSpawnPoints PlayerDropPoints";

   // Set the gameplay parameters
   %game.duration = 0;
   %game.endgameScore = 20;
   %game.endgamePause = 10;
}

function CubeMTGame::startGame(%game)
{
   parent::startGame(%game);
   
   AIFishPlayerScript::LoadActivities();
   AIFishPlayerScript::LoadEntities();
   
   TaskBar::spawn();
}

function CubeMTGame::endGame(%game)
{
   parent::endGame(%game);
}

function CubeMTGame::onGameDurationEnd()
{
   parent::onGameDurationEnd();
}

function CubeMTGame::onClientEnterGame(%game, %client)
{
   parent::onClientEnterGame(%game, %client);
   
 //  %game.SyncClock();      

   if(%game.syncClientsId)
      cancel(CubeMTGame.syncClientsId);
   %game.syncClientsId = %game.schedule(5000, "syncCubeMT");
}

function CubeMTGame::onClientLeaveGame(%game, %client)
{
   parent::onClientLeaveGame(%game, %client);

}

function CubeMTGame::syncCubeMT(%game)
{
 //  %game.SyncClock();      
    
      
   if(isObject(tod))
   {
    //  tod.setTimeOfDay(tod.time+0.001);
      tod.setTimeOfDay(0.0905);
   //   tod.play = 1;
   }
   
   // sync the scattersky for all clients
   if(isObject(ScatterSkyNetSync))
      ScatterSkyNetSync.applyChanges();
    
    
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
      if( !%cl.isAIControlled() )
      {
         schedule(6000, 0, "commandToClient",  %cl, 'ShowPlayGui' );

         if(isObject(ReefWind))
            commandToClient(%cl, 'ResetWind', %cl.getGhostID(ReefWind.getId()), 7);
      }
   }  

//   if(isObject(ReefWind))
//   {
   //   ReefWind.windEnabled = 0;
   //   ReefWind.windEnabled = 1;
   //   ReefWind.hidden = false;
//      ReefWind.resetWind();
//   }
   
   if( $Pref::Cube::QueenslandMuseum )
   {
      changeViewAllClients(6);
   }
   else if( !$Pref::Cube::demoMode )
   {
      changeViewAllClients($Pref::Cube::demoView);
   }
    
    
    
 //     %game.schedule(1000*60*5, "resyncCubeMT");

    /*  
   if(!AIFishPlayerScript.entitiesloaded)
   {
      AIFishPlayerScript::LoadActivities();
      AIFishPlayerScript::LoadEntities();
      AIFishPlayerScript.entitiesloaded = true;
   }
   */

}

function CubeMTGame::resyncCubeMT(%game)
{
   /*
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);

   } 
   */
   DugongSet1_SS.setThreadPosition(0, getThreadPosition(0));
   GreyReefSharkSet1_SS.setThreadPosition(0, getThreadPosition(0));
   GreyReefSharkSet2_SS.setThreadPosition(0, getThreadPosition(0));
   BluespottedStingRaySet1_SS.setThreadPosition(0, getThreadPosition(0));
   GreenSeaTurtleSet1_SS.setThreadPosition(0, getThreadPosition(0));
   WhitemouthMorayEelLeftSet1_SS.setThreadPosition(0, getThreadPosition(0));
   WhitemouthMorayEelRightSet1_SS.setThreadPosition(0, getThreadPosition(0));
   HermitCrab1Set1_SS.setThreadPosition(0, getThreadPosition(0));
   HermitCrab2Set1_SS.setThreadPosition(0, getThreadPosition(0));
   HermitCrab3Set1_SS.setThreadPosition(0, getThreadPosition(0));
   HermitCrab4Set1_SS.setThreadPosition(0, getThreadPosition(0));
   GrouperSet1_SS.setThreadPosition(0, getThreadPosition(0));
    
   %game.schedule(1000*60*5, "resyncCubeMT");
}


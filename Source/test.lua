SLB.using(SLB)

lm = LogicManager();

function OnGameStart()
lm:SetPlayerMaxLife(150);
lm:SetPlayerLife(10);
lm:SetPotionHealing(50);
lm:SetPlayerShootHitDamage(20);
lm:SetPlayerHitDamage(20);
lm:setSpawnPosition(0,0,10);
lm:setSpawnState(true);
playerLife = lm:GetPlayerLife();
playerMaxLife = lm:GetPlayerMaxLife();

end

function NextWave()
lm:SpawnRandomEnemies(3);
end

function PlayerDead()
lm:setSpawnState(false);
lm:DespawnEnemies();
lm:MovePlayer(0,0,0);
lm:SetPlayerLife(150);
end
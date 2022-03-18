SLB.using(SLB)

lm = LogicManager();

function OnGameStart()
lm:SetPlayerLife(150);
lm:SetPlayerMaxLife(150);
lm:SetPotionHealing(50);
lm:SetPlayerHitDamage(20);
lm:setSpawnPosition(0,0,10);
lm:MovePlayer(5,0,5);
end

function NextWave()
lm:SpawnRandomEnemies(3);
end